#include "renderview.h"

const int NUM_SAMPLES = 20;
constexpr int DEPTH = 32;

RenderView::RenderView(QWidget *parent)
    : QGraphicsView(parent)
{
    graphicsScene = new QGraphicsScene();
    setScene(graphicsScene);

    QElapsedTimer timer;
    timer.start();
    render();
    qDebug() << timer.elapsed()/1000 << "s";
}

void RenderView::render()
{
    // image setting
    constexpr int width = 640;
    constexpr int height = 360;

//    constexpr int width = 480;
//    constexpr int height = 480;

    auto fImage = createImage(width, height);

    const double screenWidth = 15.0 * width / height;
    const double screenHeight= 15.0;
    QVector3D cameraPosition(0, 0, 25);

    // Spheres
    QVector<Sphere> scene;
    scene << Sphere(QVector3D( 10, 0, 0),  4, std::make_shared<Light>());
    scene << Sphere(QVector3D(-10, 0, 0), 4, std::make_shared<Diffuse>(QVector3D(0.9, 0.1, 0.1)));
    scene << Sphere(QVector3D(  0, 0, 0),   4, std::make_shared<Glass>());
    scene << Sphere(QVector3D(0, -10004, 0), 10000, std::make_shared<Diffuse>());

    // cornellBox
    float boxSize = 16;
    float sphereRad = 3;
    QVector<Sphere> cornellBox;
    cornellBox << Sphere(QVector3D(0, -(10000+boxSize/2), 0), 10000, std::make_shared<Diffuse>()); // floor
    cornellBox << Sphere(QVector3D(0, (10000+boxSize/2), 0), 10000, std::make_shared<Diffuse>());  // ceiling
    cornellBox << Sphere(QVector3D((10000+boxSize/2), 0, 0), 10000, std::make_shared<Diffuse>(QVector3D(0.4, 0.9, 0.4)));  // right
    cornellBox << Sphere(QVector3D(-(10000+boxSize/2), 0, 0), 10000, std::make_shared<Diffuse>(QVector3D(0.9, 0.4, 0.4))); // left
    cornellBox << Sphere(QVector3D(0, 0, -(10000+boxSize/2)), 10000, std::make_shared<Diffuse>()); // front
    cornellBox << Sphere(QVector3D(0, 0, 10030), 10000, std::make_shared<Light>(QVector3D(0, 0, 0))); // back
//    cornellBox << Sphere(QVector3D(0, -(boxSize/2)+sphereRad, -3), sphereRad, std::make_shared<Diffuse>());
    cornellBox << Sphere(QVector3D(4, -(boxSize/2)+sphereRad, 3), sphereRad, std::make_shared<Diffuse>());
    cornellBox << Sphere(QVector3D(0, boxSize/2+9, 0), 10, std::make_shared<Light>(QVector3D{4, 4, 4})); // ceiling light
    cornellBox << Sphere(QVector3D(-4, -(boxSize/2)+sphereRad, 1), sphereRad, std::make_shared<Glass>());

    // Mesh
    Mesh mesh = importFbx("E:/3D Objects/bunny.fbx", 13.0f);
//    QVector3D lightBlue(199/255.0f, 241/255.0f, 255/255.0f);
//    mesh.setMaterial(std::make_shared<Diffuse>(lightBlue));
    mesh.setMaterial(std::make_shared<Glass>());

    #pragma omp parallel for schedule(dynamic, 1)
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D fColor(0, 0, 0);
            for (int n=0; n<NUM_SAMPLES; n++) {
                // screenPositionを計算
                QVector3D screenPosition;
                screenPosition[0] = (w+frand())/width * screenWidth - screenWidth/2.0;
                screenPosition[1] = (height-(h+frand()))/height * screenHeight - screenHeight/2.0;
                screenPosition[2] = cameraPosition[2] - 20;

                // rayを生成
                Ray ray(cameraPosition);
                ray.direction = (screenPosition - cameraPosition).normalized();

                // radianceを計算
                int depth = 0;
                fColor += radiance(ray, mesh, depth);
//                fColor += radiance(ray, scene, depth);
//                fColor += radiance(ray, cornellBox, depth);
            }
            fImage[h][w] = (fColor/NUM_SAMPLES);

        }
        if(omp_get_thread_num() == 0) {
            qDebug() << int(double(h)/height * 100)  << "%";
        }
    }

    gammaCorrection(fImage);

    setImage(fImage);
}


QVector3D RenderView::radiance(Ray& ray, Mesh& mesh, int& depth)
{
    static IBL sky("E:/Pictures/Textures/_HDRI/4k/rural_landscape_4k.hdr");
//    static IBL sky("E:/Pictures/Textures/_HDRI/PaperMill_E_3k.hdr");

    // シーンとの交差判定
    Intersection intersection;
    if(!mesh.intersect(ray, intersection)) return sky.getRadiance(ray);

    // ローカル座標系 (s, n, t) を作る
    auto [n, s, t] = orthonormalize(intersection.normal);

    // world座標 -> local座標
    QVector3D localDirection = worldToLocal(-ray.direction, s, n, t);

    // rayの方向とweightを計算する
    auto [nextDirection, weight] = mesh.material->sample(localDirection, depth);

    // ray更新
    ray.direction = localToWorld(nextDirection, s, n, t);
    ray.origin = intersection.position + ray.direction * 0.001f;

    // 再帰でradiance取得
    if(depth > DEPTH) return mesh.material->getEmission();
    QVector3D inRandiance = radiance(ray, mesh, depth);

    // 最終的なレンダリング方程式
    // Lo = Le + (BRDF * Li * cosθ)/pdf = Le + weight*Li
    return mesh.material->getEmission() + weight * inRandiance;
}

QVector3D RenderView::radiance(Ray& ray, QVector<Sphere>& scene, int& depth)
{
        static IBL sky("E:/Pictures/Textures/_HDRI/4k/rural_landscape_4k.hdr");
//    static IBL sky("E:/Pictures/Textures/_HDRI/PaperMill_E_3k.hdr");

    // シーンとの交差判定
    Intersection intersection;
    if(!intersectScene(ray, scene, intersection)) return sky.getRadiance(ray);

    // Hitした情報を取得
    Sphere sphere = scene[intersection.objectIndex];

    // ローカル座標系 (s, n, t) を作る
    auto [n, s, t] = orthonormalize(intersection.normal);

    // world座標 -> local座標
    QVector3D localDirection = worldToLocal(-ray.direction, s, n, t);

    // rayの方向とweightを計算する
    auto [nextDirection, weight] = sphere.material->sample(localDirection, depth);

    // ray更新
    ray.direction = localToWorld(nextDirection, s, n, t);
    ray.origin = intersection.position + ray.direction * 0.001f;

    // 再帰でradiance取得
    if(depth > DEPTH) return sphere.material->getEmission();
    QVector3D inRandiance = radiance(ray, scene, depth);

    // 最終的なレンダリング方程式
    // Lo = Le + (BRDF * Li * cosθ)/pdf = Le + weight*Li
    return sphere.material->getEmission() + weight * inRandiance;
}



void RenderView::setImage(const QVector<QVector<QVector3D>>& fImage)
{
    int width = fImage[0].size();
    int height = fImage.size();

    // set image to graphicsView
    image = new QImage(width, height, QImage::Format_RGB32);
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D fColor = fImage[h][w];

            QColor color;
            color.setRgbF(qBound(0.0f, fColor.x(), 1.0f),
                          qBound(0.0f, fColor.y(), 1.0f),
                          qBound(0.0f, fColor.z(), 1.0f));
            image->setPixelColor(w, h, color);
        }
    }
    pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    graphicsScene->addItem(pixmapItem);

    setMinimumSize(qMin(width+2, 1280), qMin(height+2, 720));
    setMaximumSize(qMin(width+2, 1280), qMin(height+2, 720));
    show();
    image->save("E:/Desktop/dev/render.png");
}


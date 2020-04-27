#include "renderview.h"

constexpr int DEPTH = 32;

RenderView::RenderView(QWidget *parent)
    : QGraphicsView(parent)
{
    graphicsScene = new QGraphicsScene();
    setScene(graphicsScene);

//    constexpr int width = 640;
//    constexpr int height = 360;
    constexpr int width = 480;
    constexpr int height = 480;
    fImage = createImage(width, height);
    setImage();

}

void RenderView::render()
{
    // image setting
    int width = fImage[0].size();
    int height = fImage.size();

    const double screenWidth = 15.0 * width / height;
    const double screenHeight= 15.0;
    QVector3D cameraPosition(0, 0, 25);

    // materials
    auto diffuse = std::make_shared<Diffuse>();
    auto redDiffuse = std::make_shared<Diffuse>(QVector3D(0.9, 0.1, 0.1));
    auto greenDiffuse = std::make_shared<Diffuse>(QVector3D(0.1, 0.9, 0.1));
    auto mirror = std::make_shared<Mirror>();
    auto glass = std::make_shared<Glass>();
    auto light = std::make_shared<Light>(5);
    auto black = std::make_shared<Light>(0);

    //------------------------------------------------------------
    // cornell box
    float boxSize = 16;
    float sphereRad = 3;
    QVector<std::shared_ptr<Object>> cornellBox;
    cornellBox << std::make_shared<Sphere>(QVector3D(0, -(10000+boxSize/2), 0), 10000, diffuse); // floor
    cornellBox << std::make_shared<Sphere>(QVector3D(0, (10000+boxSize/2), 0), 10000, diffuse);  // ceiling
    cornellBox << std::make_shared<Sphere>(QVector3D((10000+boxSize/2), 0, 0), 10000, greenDiffuse);  // right
    cornellBox << std::make_shared<Sphere>(QVector3D(-(10000+boxSize/2), 0, 0), 10000, redDiffuse); // left
    cornellBox << std::make_shared<Sphere>(QVector3D(0, 0, -(10000+boxSize/2)), 10000, diffuse); // front
    cornellBox << std::make_shared<Sphere>(QVector3D(0, 0, 10030), 10000, black); // back
//    cornellBox << std::make_shared<Sphere>(QVector3D(0, boxSize/2+9, 0), 10, light); // ceiling light
//    cornellBox << std::make_shared<Sphere>(QVector3D(4, -(boxSize/2)+sphereRad, 3), sphereRad, diffuse);
//    cornellBox << std::make_shared<Sphere>(QVector3D(-4, -(boxSize/2)+sphereRad, 1), sphereRad, glass);

    Mesh bunnyHigh = importFbx("E:/3D Objects/cornell_box/bunny.fbx");
    cornellBox << std::make_shared<BVH>(bunnyHigh);

    // area light
    areaLight = AreaLight(4, boxSize/2 - 0.01f, 5);
    cornellBox << std::make_shared<Mesh>(areaLight);
    //------------------------------------------------------------


    //------------------------------------------------------------
    // Objects
//    QVector<std::shared_ptr<Object>> objects;
//    objects << std::make_shared<Sphere>(QVector3D(-10, 0, 0), 3, mirror);
//    objects << std::make_shared<Sphere>(QVector3D(10, 0, 0), 3, light);
//    Mesh bunny = importFbx("E:/3D Objects/bunny_high.fbx");
//    objects << std::make_shared<BVH>(bunny);
    //------------------------------------------------------------

    #pragma omp parallel for schedule(dynamic, 1)
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D fColor(0, 0, 0);
            for (int n=0; n<NUM_SAMPLES; n++) {
                // screenPositionを計算
                QVector3D screenPosition;
                screenPosition[0] = (w+frand())/width * screenWidth - screenWidth/2.0;
                screenPosition[1] = (height-(h+frand()))/height * screenHeight - screenHeight/2.0;
                screenPosition[2] = cameraPosition[2] - 18;

                // rayを生成
                Ray ray(cameraPosition);
                ray.direction = (screenPosition - cameraPosition).normalized();

                // radianceを計算
                int depth = 0;
//                fColor += radiance(ray, objects, depth);
                fColor += radiance(ray, cornellBox, depth);
            }
            fColor = fColor/NUM_SAMPLES;
            fColor = gammaCorrection(fColor);

            image->setPixelColor(w, h, colorFromVector(fColor));
        }
        if(omp_get_thread_num() == 0) {
            qDebug() << int(double(h)/height * 100)  << "%";
        }
    }

    pixmapItem->setPixmap(QPixmap::fromImage(*image));

    image->save("E:/Desktop/dev/render.png");

}

QVector3D RenderView::radiance(Ray &ray, QVector<std::shared_ptr<Object>> &scene, int &depth)
{
    static IBL sky("E:/Pictures/Textures/_HDRI/4k/rural_landscape_4k.hdr");

    // シーンとの交差判定
    Intersection intersection;
    if(!intersectScene(ray, scene, intersection)) return sky.getRadiance(ray);
    std::shared_ptr<Object> obj = scene[intersection.objectIndex];

    // ローカル座標系を作る
    CoordinateConverter converter(intersection.normal);

    // world座標 -> local座標
    QVector3D localDirection = converter.toLocal(-ray.direction);

    // rayの方向とweightを計算する
    // weight = BRDF * cosθ / pdf
    auto [nextDirection, weight] = obj->material->sample(localDirection, depth);

    // ray更新
    ray.direction = converter.toWorld(nextDirection);
    ray.origin = intersection.position + ray.direction * 0.002f;

    // 再帰でradiance取得
    if(depth > DEPTH) return obj->material->getEmission();
    QVector3D inRandiance = radiance(ray, scene, depth);

    // Emittion
    QVector3D emittion = obj->material->getEmission();

    // 最終的なレンダリング方程式
    // Lo = Le + (BRDF * Li * cosθ)/pdf
    QVector3D rad = emittion + weight * inRandiance;

    return rad;
}

void RenderView::setImage()
{
    qDebug() << "setImage()";

    int width = fImage[0].size();
    int height = fImage.size();

    // set image to graphicsView
    image = new QImage(width, height, QImage::Format_RGB32);
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D fColor = fImage[h][w];
            QColor color = colorFromVector(fColor);
            image->setPixelColor(w, h, color);
        }
    }
    pixmapItem = graphicsScene->addPixmap(QPixmap::fromImage(*image));

    setMinimumSize(qMin(width+2, 1280), qMin(height+2, 720));
    setMaximumSize(qMin(width+2, 1280), qMin(height+2, 720));
//    image->save("E:/Desktop/dev/render.png");
}

void RenderView::updateImage()
{
    int width = fImage[0].size();
    int height = fImage.size();

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

    pixmapItem->setPixmap(QPixmap::fromImage(*image));
}



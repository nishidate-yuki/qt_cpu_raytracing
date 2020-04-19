#include "renderview.h"

const int NUM_SAMPLES = 200;
constexpr int DEPTH = 8;

RenderView::RenderView(QWidget *parent)
    : QGraphicsView(parent)
{
    render();
}

void RenderView::render()
{
    QElapsedTimer timer;
    timer.start();

    // image setting
    constexpr int width = 480;
    constexpr int height = 270;
    image = new QImage(width, height, QImage::Format_RGB32);
    QVector<QVector<QVector3D>> fImage(height);

    const double screenWidth = 15.0 * width / height;
    const double screenHeight= 15.0;
    QVector3D cameraPosition(0, 2, 25);

    QVector<Sphere> spheres;
    spheres << Sphere(QVector3D(10, 0, 0), 4, std::make_shared<Light>());
    spheres << Sphere(QVector3D(-10, 0, 0), 4, std::make_shared<Diffuse>());
    spheres << Sphere(QVector3D(0, 0, 0), 4, std::make_shared<Mirror>());
    spheres << Sphere(QVector3D(0, -10004, 0), 10000, std::make_shared<Diffuse>());

    #pragma omp parallel for schedule(dynamic, 1)
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D fColor(0, 0, 0);
            for (int n=0; n<NUM_SAMPLES; n++) {
                // calc screenPosition
                QVector3D screenPosition;
                screenPosition[0] = (w+frand())/width * screenWidth - screenWidth/2.0;
                screenPosition[1] = (height-(h+frand()))/height * screenHeight - screenHeight/2.0;
                screenPosition[2] = cameraPosition[2] - 20;

                // generate ray
                Ray ray(cameraPosition);
                ray.direction = (screenPosition - cameraPosition).normalized();

                // calc radiance
                int depth = 0;
                fColor += radiance(ray, spheres, depth);
            }
            fImage[h].append(fColor/NUM_SAMPLES);
        }
        qDebug() << h;
    }


    gammaCorrection(fImage);

    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D& fColor = fImage[h][w];

            QColor color;
            color.setRgbF(qBound(0.0f, fColor.x(), 1.0f),
                          qBound(0.0f, fColor.y(), 1.0f),
                          qBound(0.0f, fColor.z(), 1.0f));
            image->setPixelColor(w, h, color);
        }
    }

    graphicsScene = new QGraphicsScene();
    pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image));

    graphicsScene->addItem(pixmapItem);

    setScene(graphicsScene);
    setMinimumSize(qMin(width+2, 1280), qMin(height+2, 720));
    show();
    image->save("E:/Desktop/dev/render.png");

    qDebug() << timer.elapsed() << "ms";

}

QVector3D RenderView::radiance(Ray& ray, const QVector<Sphere>& spheres, int& depth)
{
    static IBL sky("E:/Pictures/Textures/_HDRI/4k/rural_landscape_4k.hdr");

    // シーンとの交差判定
    Intersection intersection;
    if(!ray.intersectScene(spheres, intersection)) return sky.getRadiance(ray);

    // Hitした情報を取得
    Sphere sphere = spheres[intersection.objectIndex];

    // ローカル座標系 (s, n, t) を作る
    auto [n, s, t] = orthonormalize(intersection.normal);

    // world座標 -> local座標
    QVector3D localDirection = worldToLocal(-ray.direction, s, n, t);

    // 次のrayのサンプル取得
    float pdf;
    localDirection = sphere.material->sample(localDirection, pdf, depth);

    // weightの取得
    QVector3D weight = sphere.material->getWeight(localDirection, pdf);

    // ray更新
    ray.origin = intersection.position + intersection.normal * 0.001f;
    ray.direction = localToWorld(localDirection, s, n, t);

    // 再帰でradiance取得
    if(depth > DEPTH) return sphere.material->emission;
    QVector3D inRandiance = radiance(ray, spheres, depth);

    // 最終的なレンダリング方程式
    // Lo = Le + (BRDF * Li * cosθ)/pdf = Le + wieght*Li
    return sphere.material->emission + weight*inRandiance;

}

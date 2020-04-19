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
//    spheres << Sphere(QVector3D(10, 0, 0), 4, std::make_shared<Light>(new (Light)));
//    spheres << Sphere(QVector3D(-10, 0, 0), 4, std::make_shared<Diffuse>(new (Diffuse)));
//    spheres << Sphere(QVector3D(0, 0, 0), 4, std::make_shared<Mirror>(new (Mirror)));
//    spheres << Sphere(QVector3D(0, -10004, 0), 10000, std::make_shared<Diffuse>(new (Diffuse)));
    spheres << Sphere(QVector3D(10, 0, 0), 4, std::make_shared<Light>());
    spheres << Sphere(QVector3D(-10, 0, 0), 4, std::make_shared<Diffuse>());
    spheres << Sphere(QVector3D(0, 0, 0), 4, std::make_shared<Mirror>());
    spheres << Sphere(QVector3D(0, -10004, 0), 10000, std::make_shared<Diffuse>());
//    spheres << Sphere(QVector3D(10, 0, 0), 4, (new (Light)));
//    spheres << Sphere(QVector3D(-10, 0, 0), 4, (new (Diffuse)));
//    spheres << Sphere(QVector3D(0, 0, 0), 4, (new (Mirror)));
//    spheres << Sphere(QVector3D(0, -10004, 0), 10000, (new (Diffuse)));
//    spheres << Sphere{QVector3D(10, 0, 0), 4};
//    spheres << Sphere{QVector3D(-10, 0, 0), 4};
//    spheres << Sphere{QVector3D(0, 0, 0), 4};
//    spheres << Sphere{QVector3D(0, -10004, 0), 10000};

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


    // hitしなかったらbackgroundを返す
    Intersection intersection;
    if(!ray.intersectScene(spheres, intersection)){
        return sky.getRadiance(ray);
    }

    // hitした
    Sphere sphere = spheres[intersection.objectIndex];
    Hitpoint hitpoint = intersection.hitpoint;


    QVector3D normal = hitpoint.normal;

    // ローカル座標系 (s, n, t) を作る
    QVector3D n = normal;
    QVector3D s, t;
    orthonormalize(n, s, t);

    // world -> local
    QVector3D localDirection = worldToLocal(-ray.direction, s, n, t);

    // 次のrayのサンプル取得
    float tmp;
    localDirection = sphere.material->sample(localDirection, tmp, depth);

    // weightの取得
    QVector3D weight = sphere.material->getWeight(localDirection, tmp);

    // update ray
    ray.origin = hitpoint.position + normal * 0.001f;
    ray.direction = localToWorld(localDirection, s, n, t);

    // depthチェック
    if(depth > DEPTH){
        return sphere.material->emission;
    }

    // 再帰
    QVector3D inRandiance = radiance(ray, spheres, depth);

    // Lo = Le + (BRDF * Li * cosθ)/pdf
    //    = Le + wieght*Li
    return sphere.material->emission + weight*inRandiance;


//    for (int depth = 0; depth<DEPTH; depth++) {
//        // hitしなかったらbackgroundを返す
//        Intersection intersection;
//        if(!ray.intersectScene(spheres, intersection)){
//            ray.emission = sky.getRadiance(ray);
//            break;
//        }

//        Sphere sphere = spheres[intersection.objectIndex];
//        Hitpoint hitpoint = intersection.hitpoint;

//        QVector3D normal = hitpoint.normal;

//        // ローカル座標系 (s, n, t) を作る
//        QVector3D n = normal;
//        QVector3D s, t;
//        orthonormalize(n, s, t);

//        // world -> local
//        QVector3D localDirection = worldToLocal(-ray.direction, s, n, t);

//        float tmp;
//        localDirection = sphere.material->sample(localDirection, tmp);

//        // update ray
//        ray.origin = hitpoint.position + normal * 0.001f;
//        ray.direction = localToWorld(localDirection, s, n, t);
////        ray.scatter *= sphere.material->scatter;
////        ray.emission *= sphere.material->emission;

//        // Light
////        if(sphere.material.materialType == LIGHT){
////            ray.emission = sphere.material.emission;
////            break;
////        }

//        // Diffuse
////        else if(sphere.material.materialType == DIFFUSE){

////            // ローカル座標系 (s, n, t) を作る
////            // 完全拡散反射はローカル座標系のレイは必要ない
////            // サンプリングしたレイをワールドに戻すために使う
////            QVector3D n = normal;
////            QVector3D s, t;
////            // TODO: s, n, t = orthonormalize(n)にする
////            orthonormalize(n, s, t);

////            // importance sampling
////            float phi = 2 * M_PI * frand();
////            float y = sqrt(frand());
////            float d = sqrt(1 - y*y);
////            float x = d * cos(phi);
////            float z = d * sin(phi);

////            QVector3D localDirection(x, y, z);

////            // update ray
////            ray.origin = hitpoint.position + normal * 0.001f;
////            ray.direction = localToWorld(localDirection, s, n, t);
////            ray.emission = QVector3D(0, 0, 0);
////        }

////        else if(sphere.material.materialType == SPECULAR){
////            // ローカル座標系 (s, n, t) を作る
////            // 完全拡散反射はローカル座標系のレイは必要ない
////            // サンプリングしたレイをワールドに戻すために使う
////            QVector3D n = normal;
////            QVector3D s, t;
////            orthonormalize(n, s, t);

////            // world -> local
////            QVector3D localDirection = worldToLocal(-ray.direction, s, n, t);

////            // sampling
////            localDirection = reflect(localDirection, QVector3D(0, 1, 0));

////            // update ray
////            ray.origin = hitpoint.position + normal * 0.001f;
////            ray.direction = localToWorld(localDirection, s, n, t);
////            ray.emission = QVector3D(0, 0, 0);
////        }

////        // Normal
////        else{
////            ray.emission = normal/2 + QVector3D(0.5, 0.5, 0.5);
////            break;
////        }



//    }

//    return ray.scatter * ray.emission;

}

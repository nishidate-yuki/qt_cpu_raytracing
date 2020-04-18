#include "renderview.h"

const int NUM_SAMPLES = 64;
const int DEPTH = 8;

RenderView::RenderView(QWidget *parent)
    : QGraphicsView(parent)
{
    render();
}

void RenderView::render()
{
    // image setting
    const int width = 480;
    const int height = 360;
    image = new QImage(width, height, QImage::Format_RGB32);

    const double screenWidth = 15.0 * width / height;
    const double screenHeight= 15.0;

    QVector3D cameraPosition(0, 0, 15);

    Material light;
    light.emission = QVector3D(1, 1, 1);
    light.materialType = LIGHT;

    Material diffuse;
    diffuse.scatter = QVector3D(1, 1, 1);
    diffuse.emission = QVector3D(0, 0, 0);
    diffuse.materialType = DIFFUSE;

    QVector<Sphere> spheres;
    spheres << Sphere{QVector3D(0, 0, 0), 4, light};
    spheres << Sphere{QVector3D(0, -10004, 0), 10000, diffuse};

    #pragma omp parallel for
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D fColor(0, 0, 0);
            for (int n=0; n<NUM_SAMPLES; n++) {

                // raytracing
                QVector3D screenPosition;
                screenPosition[0] = (w+frand())/width * screenWidth - screenWidth/2.0;
                screenPosition[1] = (height-(h+frand()))/height * screenHeight - screenHeight/2.0;
                screenPosition[2] = cameraPosition[2] - 10;

                Ray ray(cameraPosition);
                ray.direction = (screenPosition - cameraPosition).normalized();

                fColor += radiance(ray, spheres);
            }
            QColor color;
            color.setRedF(fColor[0]/NUM_SAMPLES);
            color.setGreenF(fColor[1]/NUM_SAMPLES);
            color.setBlueF(fColor[2]/NUM_SAMPLES);
            image->setPixelColor(w, h, color);

        }
        qDebug() << h;
    }

    scene = new QGraphicsScene();
    pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image));

    scene->addItem(pixmapItem);

    setScene(scene);
    setMinimumSize(qMin(width+2, 1280), qMin(height+2, 720));
    show();
}

QVector3D RenderView::radiance(Ray& ray, const QVector<Sphere>& spheres)
{
    QVector3D backgroundColor(0.0, 0.0, 0.0f);

    for (int depth = 0; depth<DEPTH; depth++) {
        // hitしなかったらbackgroundを返す
        Intersection intersection;
        if(!ray.intersectScene(spheres, intersection)){
            ray.emission = backgroundColor;
            break;
        }

//        qDebug() << "ID:" << intersection.objectIndex;

        Sphere sphere = spheres[intersection.objectIndex];
        Hitpoint hitpoint = intersection.hitpoint;

        QVector3D normal = hitpoint.normal;
        // sphereの内側からrayが出ていく場合
//        if(dot(normal, ray.direction) < 0){
//            normal *= -1.0;
//        }

        // Light
        if(sphere.material.materialType == LIGHT){
            ray.emission = sphere.material.emission;
            break;
        }

        // Diffuse
        if(sphere.material.materialType == DIFFUSE){

            // ローカル座標系 (s, n, t) を作る
            // 完全拡散反射はローカル座標系のレイは必要ない
            // サンプリングしたレイをワールドに戻すために使う
            QVector3D n = normal;
            QVector3D s, t;
            orthonormalize(n, s, t);

            float theta = 0.5 * std::acos(1 - 2*frand());
            float phi = 2*M_PI * frand();

            float x = cos(phi) * sin(theta);
            float y = cos(theta);
            float z = sin(phi) * sin(theta);

            QVector3D localNextDirection(x, y, z);

            ray.origin = hitpoint.position + normal * 0.001f;
            ray.direction = localToWorld(localNextDirection, s, n, t);

            ray.emission = QVector3D(0, 0, 0);
        }

        // Normal
        else{
            ray.emission = normal/2 + QVector3D(0.5, 0.5, 0.5);
            break;
        }

    }

    return ray.scatter * ray.emission;


    // simple shading
//    QVector3D lightPosition(10, 10, 10);
//    float brightness = dot(hitpoint.normal, (lightPosition-hitpoint.position).normalized());
//    brightness = qMax(brightness, 0.0f);
//    return QVector3D(brightness, brightness, brightness);
}

#include "renderview.h"

const int NUM_SAMPLES = 200;
const int DEPTH = 8;

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

    Material light;
    light.emission = QVector3D(1, 1, 1);
    light.materialType = LIGHT;

    Material diffuse;
    diffuse.scatter = QVector3D(1, 1, 1);
    diffuse.emission = QVector3D(0, 0, 0);
    diffuse.materialType = DIFFUSE;

    Material mirror;
    mirror.scatter = QVector3D(0, 0, 0);
    mirror.emission = QVector3D(0, 0, 0);
    mirror.materialType = SPECULAR;

    QVector<Sphere> spheres;
    spheres << Sphere{QVector3D(10, 0, 0), 4, light};
    spheres << Sphere{QVector3D(-10, 0, 0), 4, diffuse};
    spheres << Sphere{QVector3D(0, 0, 0), 4, mirror};
    spheres << Sphere{QVector3D(0, -10004, 0), 10000, diffuse};

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
                fColor += radiance(ray, spheres);
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

    scene = new QGraphicsScene();
    pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image));

    scene->addItem(pixmapItem);

    setScene(scene);
    setMinimumSize(qMin(width+2, 1280), qMin(height+2, 720));
    show();
    image->save("E:/Desktop/dev/render.png");

    qDebug() << timer.elapsed() << "ms";

}

QVector3D RenderView::radiance(Ray& ray, const QVector<Sphere>& spheres)
{
    static IBL sky("E:/Pictures/Textures/_HDRI/4k/rural_landscape_4k.hdr");

    for (int depth = 0; depth<DEPTH; depth++) {
        // hitしなかったらbackgroundを返す
        Intersection intersection;
        if(!ray.intersectScene(spheres, intersection)){
            ray.emission = sky.getRadiance(ray);
            break;
        }

        Sphere sphere = spheres[intersection.objectIndex];
        Hitpoint hitpoint = intersection.hitpoint;

        QVector3D normal = hitpoint.normal;

        // Light
        if(sphere.material.materialType == LIGHT){
            ray.emission = sphere.material.emission;
            break;
        }

        // Diffuse
        else if(sphere.material.materialType == DIFFUSE){

            // ローカル座標系 (s, n, t) を作る
            // 完全拡散反射はローカル座標系のレイは必要ない
            // サンプリングしたレイをワールドに戻すために使う
            QVector3D n = normal;
            QVector3D s, t;
            orthonormalize(n, s, t);

            // importance sampling
            float phi = 2 * M_PI * frand();
            float y = sqrt(frand());
            float d = sqrt(1 - y*y);
            float x = d * cos(phi);
            float z = d * sin(phi);

            QVector3D localDirection(x, y, z);

            // update ray
            ray.origin = hitpoint.position + normal * 0.001f;
            ray.direction = localToWorld(localDirection, s, n, t);
            ray.emission = QVector3D(0, 0, 0);
        }

        else if(sphere.material.materialType == SPECULAR){
            // ローカル座標系 (s, n, t) を作る
            // 完全拡散反射はローカル座標系のレイは必要ない
            // サンプリングしたレイをワールドに戻すために使う
            QVector3D n = normal;
            QVector3D s, t;
            orthonormalize(n, s, t);

            // world -> local
            QVector3D localDirection = worldToLocal(-ray.direction, s, n, t);

            // sampling
            localDirection = reflect(localDirection, QVector3D(0, 1, 0));

            // update ray
            ray.origin = hitpoint.position + normal * 0.001f;
            ray.direction = localToWorld(localDirection, s, n, t);
            ray.emission = QVector3D(0, 0, 0);
        }

        // Normal
        else{
            ray.emission = normal/2 + QVector3D(0.5, 0.5, 0.5);
            break;
        }

    }

    return ray.scatter * ray.emission;

}

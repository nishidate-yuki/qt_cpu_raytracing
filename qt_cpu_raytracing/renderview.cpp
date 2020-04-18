#include "renderview.h"

RenderView::RenderView(QWidget *parent)
    : QGraphicsView(parent)
{
    render();
}

void RenderView::render()
{
//    QString filename = "E:/Desktop/dev/bird.jpg";
//    image = new QImage(filename);

    // image setting
    const int width = 480;
    const int height = 360;
    image = new QImage(width, height, QImage::Format_RGB32);


    const double screenWidth = 15.0 * width / height;
    const double screenHeight= 15.0;
    const float ambientLight = 0.1;

    QVector3D cameraPosition(0, 0, 15);
    QVector3D lightPosition(10, 10, 10);
//    Sphere sphere = {QVector3D(0, 0, 0), 7};

    QVector<Sphere> spheres;
    spheres << Sphere{QVector3D(-4, 0, 0), 5};
    spheres << Sphere{QVector3D(4, 0, 0), 5};

    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
//            for (int n=0; n<8; n++) {

//            }
            // raytracing
            QVector3D screenPosition;
            screenPosition[0] = float(w)/width * screenWidth - screenWidth/2.0;
            screenPosition[1] = float(height-h)/height * screenHeight - screenHeight/2.0;
            screenPosition[2] = cameraPosition[2] - 10;

            Ray ray(cameraPosition);
            ray.direction = (screenPosition - cameraPosition).normalized();

            QColor color;
            QVector3D fColor = radiance(ray, spheres, 8);
            color.setRedF(fColor[0]);
            color.setGreenF(fColor[1]);
            color.setBlueF(fColor[2]);
//            if (ray.intersectScene(spheres, intersection)){
//                float brightness = QVector3D::dotProduct(hitpoint.normal, lightPosition.normalized());
//                brightness = qBound(ambientLight, brightness+ambientLight, float(1.0));
//                color = QColor(brightness*255, brightness*255, brightness*255);
//            }else{
//                color = QColor(0, 0, 0);
//            }

            image->setPixelColor(w, h, color);
        }
    }

    scene = new QGraphicsScene();
    pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image));

    scene->addItem(pixmapItem);

    setScene(scene);
    setMinimumSize(qMin(width+2, 1280), qMin(height+2, 720));
    show();
}

QVector3D RenderView::radiance(Ray& ray, const QVector<Sphere>& spheres, const int depth)
{
    QVector3D backgroundColor(0.0, 0.0, 0.3f);
    Intersection intersection;
    if(!ray.intersectScene(spheres, intersection)){
        return backgroundColor;
    }

    QVector3D lightPosition(10, 10, 10);
    Sphere spehre = spheres[intersection.objectIndex];
    Hitpoint hitpoint = intersection.hitpoint;

    float brightness = QVector3D::dotProduct(hitpoint.normal, (lightPosition-hitpoint.position).normalized());
    brightness = qMax(brightness, 0.0f);
    return QVector3D(brightness, brightness, brightness);
}

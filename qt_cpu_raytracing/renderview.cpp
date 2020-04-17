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
    Sphere sphere = {QVector3D(0, 0, 0), 7};

    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {

            // raytracing
            QVector3D screenPosition;
            screenPosition[0] = float(w)/width * screenWidth - screenWidth/2.0;
            screenPosition[1] = float(height-h)/height * screenHeight - screenHeight/2.0;
            screenPosition[2] = cameraPosition[2] - 10;

            Ray ray(cameraPosition);
            ray.direction = (screenPosition - cameraPosition).normalized();

            QColor color;
            Hitpoint hitpoint;
            if (ray.intersect(sphere, hitpoint)){
                float brightness = QVector3D::dotProduct(hitpoint.normal, lightPosition.normalized());
                brightness = qBound(ambientLight, brightness+ambientLight, float(1.0));
                color = QColor(brightness*255, brightness*255, brightness*255);
            }else{
                color = QColor(0, 0, 0);
            }

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

#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QtDebug>
#include <QRandomGenerator>
#include <QtMath>
#include <QElapsedTimer>
#include <omp.h>
#include "ray.h"
#include "utils.h"
#include "sky.h"
#include "sphere.h"
#include "triangle.h"
#include "mesh.h"
#include "object.h"
#include "coordinateconverter.h"
#include "bvh.h"

class RenderView : public QGraphicsView
{
    Q_OBJECT

public:
    RenderView(QWidget *parent);

    void render();
    QVector3D radiance(Ray& ray, Mesh& mesh, int& depth);
    QVector3D radiance(Ray&, QVector<Sphere>&, int&);
    QVector3D radiance(Ray&, QVector<std::shared_ptr<Object>>&, int&);
    QVector3D radianceIBL(Ray&, QVector<std::shared_ptr<Object>>&, int&);
    void setImage();
    void updateImage();

    int NUM_SAMPLES = 100;

private:
    QGraphicsScene* graphicsScene;
    QGraphicsPixmapItem* pixmapItem;
    QImage* image;
    QVector<QVector<QVector3D>> fImage;
    AreaLight areaLight;
};

#endif // RENDERVIEW_H

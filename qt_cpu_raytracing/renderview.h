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

class RenderView : public QGraphicsView
{
    Q_OBJECT

public:
    RenderView(QWidget *parent);

    void render();
    QVector3D radiance(Ray&, const QVector<Sphere>&);

private:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pixmapItem;
    QImage* image;
//    IBL* sky;
};

#endif // RENDERVIEW_H

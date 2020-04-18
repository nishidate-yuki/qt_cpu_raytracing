#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QtDebug>
#include <QVector3D>

#include "ray.h"

class RenderView : public QGraphicsView
{
    Q_OBJECT

public:
    RenderView(QWidget *parent);

    void render();
    QVector3D radiance(Ray&, const QVector<Sphere>&,const int);

private:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pixmapItem;
    QImage* image;
};

#endif // RENDERVIEW_H

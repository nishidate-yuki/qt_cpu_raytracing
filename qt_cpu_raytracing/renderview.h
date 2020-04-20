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

class RenderView : public QGraphicsView
{
    Q_OBJECT

public:
    RenderView(QWidget *parent);

    void render();
    QVector3D radiance(Ray& ray, Mesh& mesh, int& depth);
    QVector3D radiance(Ray&, QVector<Sphere>&, int&);
    void setImage(const QVector<QVector<QVector3D>>&);

private:
    QGraphicsScene* graphicsScene;
    QGraphicsPixmapItem* pixmapItem;
    QImage* image;
};

#endif // RENDERVIEW_H

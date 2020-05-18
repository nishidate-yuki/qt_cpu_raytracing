#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QWidget>
#include <QtDebug>
#include <QThread>
#include <QtMath>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
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
#include "renderer.h"

class RenderView : public QGraphicsView
{
    Q_OBJECT

public:
    RenderView(QWidget *parent);

    ~RenderView();

    Renderer *renderer;

private:
    QGraphicsScene* graphicsScene;
    QGraphicsPixmapItem* pixmapItem;
    QThread renderThread;

public slots:
    void setImage(QImage);

signals:
    void renderRequested();
};

#endif // RENDERVIEW_H

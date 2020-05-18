#ifndef RENDERER_H
#define RENDERER_H

#include <QWidget>
#include <QtDebug>
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

class Renderer : public QObject
{
    Q_OBJECT
public:
    Renderer(int width, int height);
    int num_samples = 100;

public slots:
    void render();

signals:
    void finished(QImage image);

private:
    int width;
    int height;
    AreaLight areaLight;

    QVector3D radiance(Ray&, QVector<std::shared_ptr<Object>>&, int&);
    QVector3D radianceIBL(Ray&, QVector<std::shared_ptr<Object>>&, int&);

};

#endif // RENDERER_H

#include "material.h"


QVector3D reflect(const QVector3D& v, const QVector3D& n) {
    return -v + 2*dot(v, n)*n;
}

Diffuse::Diffuse()
{
    scatter = QVector3D(1.0, 1.0, 1.0);
    emission = QVector3D(0, 0, 0);
}

QVector3D Diffuse::sample(const QVector3D &direction, double &pdf) const
{
    // この関数の中はすべてローカル座標系と考えていい

    // importance sampling
    float phi = 2 * M_PI * frand();
    float y = sqrt(frand());
    float d = sqrt(1 - y*y);
    float x = d * cos(phi);
    float z = d * sin(phi);

    return QVector3D(x, y, z);
}

Mirror::Mirror()
{
    scatter = QVector3D(0, 0, 0);
    emission = QVector3D(0, 0, 0);
}

QVector3D Mirror::sample(const QVector3D &direction, double &pdf) const
{
    return reflect(direction, QVector3D(0, 1, 0));
}

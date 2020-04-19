#ifndef RAY_H
#define RAY_H

#include <QVector3D>
#include "material.h"

struct Intersection {
    float distance;
    QVector3D position;
    QVector3D normal;
    int objectIndex;

    Intersection() : distance(FLT_MAX), objectIndex(-1) {}
};

class Ray {
public:
    Ray(const QVector3D&);
    Ray(const QVector3D&, const QVector3D&);

    QVector3D origin;
    QVector3D direction;
};

#endif // RAY_H

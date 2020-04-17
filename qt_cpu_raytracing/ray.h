#ifndef RAY_H
#define RAY_H

#include <QVector3D>

struct Sphere {
    QVector3D center;
    float radius;
};

struct Hitpoint{
    float distance;
    QVector3D position;
    QVector3D normal;
    int material;

    Hitpoint() : distance(FLT_MAX), position(), normal() {}
};


class Ray {
public:
    Ray(const QVector3D&);
    Ray(const QVector3D&, const QVector3D&);

    bool intersect(const Sphere&, Hitpoint&);

    QVector3D origin;
    QVector3D direction;
    QVector3D scatter;
    QVector3D emission;
};

#endif // RAY_H

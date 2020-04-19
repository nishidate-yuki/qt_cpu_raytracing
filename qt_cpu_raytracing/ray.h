#ifndef RAY_H
#define RAY_H

#include <QVector3D>
#include "material.h"
#include "sphere.h"

struct Hitpoint{
    float distance;
    QVector3D position;
    QVector3D normal;
//    int material;

    Hitpoint() : distance(FLT_MAX), position(), normal() {}
};

struct Intersection {
    float distance;
    QVector3D position;
    QVector3D normal;
    int objectIndex;

    Intersection() : distance(FLT_MAX), objectIndex(-1) {}
};

//struct Intersection {
//    Hitpoint hitpoint;
//    int objectIndex;

//    Intersection() : objectIndex(-1) {}
//};


//const Sphere scene[] = {
//    Sphere{QVector3D(0, 0, 0), 7}
//};


class Ray {
public:
    Ray(const QVector3D&);
    Ray(const QVector3D&, const QVector3D&);

    bool intersect(const Sphere&, Intersection&);
    bool intersectScene(const QVector<Sphere>&, Intersection&);

    QVector3D origin;
    QVector3D direction;
    QVector3D scatter;
    QVector3D emission;
};

#endif // RAY_H

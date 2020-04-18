#ifndef RAY_H
#define RAY_H

#include <QVector3D>

enum ReflectionType {
    DIFFUSE,	// 完全拡散面
    SPECULAR,	// 完全鏡面
    REFRACTION,	// 完全なガラス
};

struct Material{
    QVector3D scatter;
    QVector3D emission;
    ReflectionType reflectionType;

    Material() :
        scatter(1.0, 1.0, 1.0), emission(0, 0, 0),
        reflectionType(DIFFUSE) {}
};

struct Sphere {
    QVector3D center;
    float radius;
    Material material;
};

struct Hitpoint{
    float distance;
    QVector3D position;
    QVector3D normal;
    int material;

    Hitpoint() : distance(FLT_MAX), position(), normal() {}
};

struct Intersection {
    Hitpoint hitpoint;
    int objectIndex;

    Intersection() : objectIndex(-1) {}
};


//const Sphere scene[] = {
//    Sphere{QVector3D(0, 0, 0), 7}
//};


class Ray {
public:
    Ray(const QVector3D&);
    Ray(const QVector3D&, const QVector3D&);

    bool intersect(const Sphere&, Hitpoint&);
    bool intersectScene(const QVector<Sphere>&, Intersection&);

    QVector3D origin;
    QVector3D direction;
    QVector3D scatter;
    QVector3D emission;
};

#endif // RAY_H

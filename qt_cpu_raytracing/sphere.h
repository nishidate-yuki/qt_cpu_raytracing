#ifndef SPHERE_H
#define SPHERE_H

#define QT_FOR_EACH_AUTOMATIC_TEMPLATE_SMART_POINTER

#include <QVector3D>
#include <QString>
#include <QSharedDataPointer>
#include <memory>
#include "material.h"

//struct Sphere {
//    QVector3D center;
//    float radius;
//    QSharedPointer<Material> material;
//};

class Sphere
{
public:
    Sphere();

    QVector3D center;
    float radius;
    std::unique_ptr<Material> material;
};

#endif // SPHERE_H

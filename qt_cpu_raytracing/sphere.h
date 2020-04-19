#ifndef SPHERE_H
#define SPHERE_H

#define QT_FOR_EACH_AUTOMATIC_TEMPLATE_SMART_POINTER

#include <QVector3D>
#include <QString>
#include <QSharedDataPointer>
#include <memory>
#include <typeinfo>
#include "material.h"

//struct Sphere {
//    QVector3D center;
//    float radius;
//    QSharedPointer<Material> material;
//};

class Sphere
{
public:
    Sphere(const QVector3D& center, const float& radius);
    Sphere(const QVector3D& center, const float& radius, const std::shared_ptr<Material>& material);
//    Sphere(const QVector3D& center, const float& radius, Material* material);

    QVector3D center;
    float radius;
    std::shared_ptr<Material> material;
};

#endif // SPHERE_H

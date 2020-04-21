#ifndef SPHERE_H
#define SPHERE_H

#define QT_FOR_EACH_AUTOMATIC_TEMPLATE_SMART_POINTER

#include <QVector3D>
#include <QString>
#include <QSharedDataPointer>
#include <memory>
#include <typeinfo>
#include "material.h"
#include "ray.h"
#include "object.h"

class Sphere : public Object
{
public:
    Sphere(const QVector3D&, const float&);
    Sphere(const QVector3D&, const float&, const std::shared_ptr<Material>&);

    bool intersect(const Ray&, Intersection&) override;

    QVector3D center;
    float radius;
//    std::shared_ptr<Material> material;

    void setMaterial(const std::shared_ptr<Material>&);
};

bool intersectScene(const Ray&, QVector<Sphere>&, Intersection&);

#endif // SPHERE_H

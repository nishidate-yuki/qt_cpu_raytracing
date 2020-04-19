#include "sphere.h"

Sphere::Sphere(const QVector3D &center, const float &radius)
    : center(center),
      radius(radius),
      material(new Diffuse)
{
}

Sphere::Sphere(const QVector3D &center, const float &radius, const std::shared_ptr<Material>& material)
    : center(center),
      radius(radius),
      material(material)
{
}

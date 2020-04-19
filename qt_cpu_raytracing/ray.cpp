#include "ray.h"
#include "renderview.h"

Ray::Ray(const QVector3D& o)
    : origin(o),
      direction(-o),
      scatter(1.0, 1.0, 1.0),
      emission(0, 0, 0)
{

}

Ray::Ray(const QVector3D& o, const QVector3D& d)
    : origin(o),
      direction(d.normalized()),
      scatter(1.0, 1.0, 1.0),
      emission(0, 0, 0)
{
}

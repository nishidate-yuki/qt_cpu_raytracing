#include "ray.h"
#include "renderview.h"

Ray::Ray(const QVector3D& o)
    : origin(o),
      direction(-o)
{

}

Ray::Ray(const QVector3D& o, const QVector3D& d)
    : origin(o),
      direction(d.normalized())
{
}

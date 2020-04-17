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

bool Ray::intersect(const Sphere &sphere, Hitpoint& hitpoint)
{
    QVector3D rayToSphere = origin - sphere.center;
    float a = QVector3D::dotProduct(direction, direction);
    float b = QVector3D::dotProduct(rayToSphere, direction);
    float c = QVector3D::dotProduct(rayToSphere, rayToSphere)
              - sphere.radius * sphere.radius;
    float d = b*b - a*c;

    if(d < 0){
        return false;
    }

    float t1 = -b - sqrt(d); // small t
    float t2 = -b + sqrt(d); // big t

    // Rayの後ろ方向でヒットしていても更新しない
    // 既にもっと近い地点でヒットしていたら更新しない
    if(0 < t1 && t1 < hitpoint.distance){
        hitpoint.distance = t1;
        hitpoint.position = origin + t1 * direction;
        hitpoint.normal = (hitpoint.position - sphere.center).normalized();
        return true;
    }
    if(0 < t2 && t2 < hitpoint.distance){
        hitpoint.distance = t2;
        hitpoint.position = origin + t2 * direction;
        hitpoint.normal = (hitpoint.position - sphere.center).normalized();
        return true;
    }

    return false;
}

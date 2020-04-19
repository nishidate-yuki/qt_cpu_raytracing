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

bool Ray::intersect(const Sphere &sphere, Intersection& hitpoint)
{
    QVector3D rayToSphere = origin - sphere.center;
    float a = QVector3D::dotProduct(direction, direction);
    float b = QVector3D::dotProduct(rayToSphere, direction);
    float c = QVector3D::dotProduct(rayToSphere, rayToSphere)
              - sphere.radius * sphere.radius;
    float discriminant = b*b - a*c;

    if(discriminant < 0){
        return false;
    }

    float t1 = -b - sqrt(discriminant); // small t
    float t2 = -b + sqrt(discriminant); // big t

    // Rayの後ろ方向でヒットしていても更新しない
    // 既にもっと近い地点でヒットしているかは無視する
    if(0 < t1){
        hitpoint.distance = t1;
        hitpoint.position = origin + t1 * direction;
        hitpoint.normal = (hitpoint.position - sphere.center).normalized();
        return true;
    }
    if(0 < t2){
        hitpoint.distance = t2;
        hitpoint.position = origin + t2 * direction;
        hitpoint.normal = (hitpoint.position - sphere.center).normalized();
        return true;
    }

    return false;
}

bool Ray::intersectScene(const QVector<Sphere>& scene, Intersection& intersection)
{
    for (int i=0; i<scene.size(); i++) {
        Intersection hitpoint;
        if(intersect(scene[i], hitpoint)){
            if(hitpoint.distance < intersection.distance){
                intersection = hitpoint;
                intersection.objectIndex = i;
            }
        }
    }
    return intersection.objectIndex != -1;
}

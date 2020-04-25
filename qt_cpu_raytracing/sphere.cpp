#include "sphere.h"

Sphere::Sphere(const QVector3D &center, const float &radius)
    : center(center),
      radius(radius)
{
}

Sphere::Sphere(const QVector3D &center, const float &radius, const std::shared_ptr<Material>& material)
    : center(center),
      radius(radius)
{
    this->material = material;
}

bool Sphere::intersect(const Ray &ray, Intersection& intersection)
{
    QVector3D rayToSphere = ray.origin - center;
    float a = QVector3D::dotProduct(ray.direction, ray.direction);
    float b = QVector3D::dotProduct(rayToSphere, ray.direction);
    float c = QVector3D::dotProduct(rayToSphere, rayToSphere)
              - radius * radius;
    float discriminant = b*b - a*c;

    if(discriminant < 0){
        return false;
    }

    float t1 = -b - sqrt(discriminant); // small t
    float t2 = -b + sqrt(discriminant); // big t

    // Rayの後ろ方向でヒットしていても更新しない
    // 既にもっと近い地点でヒットしているかは無視する
    if(0 < t1){
        intersection.distance = t1;
        intersection.position = ray.origin + t1 * ray.direction;
        intersection.normal = (intersection.position - center).normalized();
        return true;
    }
    if(0 < t2){
        intersection.distance = t2;
        intersection.position = ray.origin + t2 * ray.direction;
        intersection.normal = (intersection.position - center).normalized();
        return true;
    }

    return false;
}

#include "sphere.h"

Sphere::Sphere(const QVector3D &center, const float &radius)
    : center(center),
      radius(radius),
      material(new Diffuse)
{
//        material = std::make_shared<Diffuse>(new Diffuse());
}

Sphere::Sphere(const QVector3D &center, const float &radius, const std::shared_ptr<Material>& material)
    : center(center),
      radius(radius),
      material(material)
{
    //    this->material = material;
}

//Sphere::Sphere(const QVector3D &center, const float &radius, Material *material)
//    : center(center),
//      radius(radius)
//{
//    this->material = std::make_shared<Material>(material);
//}

//Sphere::Sphere(const QVector3D &center, const float &radius, Material* materialPtr)
//    : center(center),
//      radius(radius)
//{
//    if(typeid(*materialPtr) == typeid(Diffuse)){
//        material = std::make_unique<Diffuse>(materialPtr);
//    }
//    else if(typeid(*materialPtr) == typeid(Mirror)){
//        material = std::make_unique<Mirror>(materialPtr);
//    }

//    decltype(materialPtr) type ;
//}

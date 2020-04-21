#include "object.h"

Object::Object()
    : material(new Diffuse)
{

}

bool intersectScene(const Ray &ray, QVector<std::shared_ptr<Object>> &scene, Intersection &intersection)
{
    for (int i=0; i<scene.size(); i++) {
        Intersection hitpoint;
        if(scene[i]->intersect(ray, hitpoint)){
            if(hitpoint.distance < intersection.distance){
                intersection = hitpoint;
                intersection.objectIndex = i;
            }
        }
    }
    return intersection.objectIndex != -1;
}

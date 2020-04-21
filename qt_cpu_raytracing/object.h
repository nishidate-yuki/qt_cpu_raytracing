#ifndef OBJECT_H
#define OBJECT_H

#include "ray.h"



class Object
{
public:
    Object();

    std::shared_ptr<Material> material;

    virtual bool intersect(const Ray&, Intersection&) = 0;
};


bool intersectScene(const Ray&, QVector<std::shared_ptr<Object>>&, Intersection&);

#endif // OBJECT_H

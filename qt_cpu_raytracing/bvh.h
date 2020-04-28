#ifndef BVH_H
#define BVH_H

#include <tuple>
#include <QVector>
#include <algorithm>
#include <QElapsedTimer>
#include "triangle.h"
#include "mesh.h"
#include "ray.h"

// BB: BoundingBox
class BoundingBox
{
public:
    QVector3D min;
    QVector3D max;

    BoundingBox():
        min{ FLT_MAX, FLT_MAX, FLT_MAX },
        max{ -FLT_MAX, -FLT_MAX, -FLT_MAX }
    {
    }

    bool intersect(const Ray &ray);

};

// BVH のノードの構造
class BVHnode
{
public:
    BoundingBox bbox;
    std::shared_ptr<BVHnode> left, right;
    std::shared_ptr<Triangle> triangle;

    BVHnode(): left(nullptr), right(nullptr), triangle(nullptr){}

    bool isLeaf();
    bool intersect(const Ray &ray, Intersection &intersection);
};

class BVH : public Object
{
public:
    BVH(Mesh&);

    bool intersect(const Ray &ray, Intersection &intersection) override;

private:
    std::shared_ptr<BVHnode> root;

};

#endif // BVH_H

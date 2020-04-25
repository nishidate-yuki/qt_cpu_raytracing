#ifndef BVH_H
#define BVH_H

#include <QVector>
#include <algorithm>
#include "triangle.h"
#include "mesh.h"

// BB: BoundingBox
struct BoundingBox
{
    QVector3D min;
    QVector3D max;

    BoundingBox():
        min{ FLT_MAX, FLT_MAX, FLT_MAX },
        max{ -FLT_MAX, -FLT_MAX, -FLT_MAX }
    {
    }
};

// BVH のノードの構造
struct BVHnode
{
    BoundingBox bbox;
    std::shared_ptr<BVHnode> left, right;
    std::shared_ptr<Triangle> triangle;

    BVHnode(): left(nullptr), right(nullptr), triangle(nullptr){}
};

class BVH
{
public:
    BVH(Mesh&);

private:
    std::shared_ptr<BVHnode> root;

};

#endif // BVH_H

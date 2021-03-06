#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QtDebug>
#include <iostream>
#include "fbxsdk.h"
#include "triangle.h"
#include "object.h"

struct Vertex{
    QVector3D position;
    QVector3D normal;
//    QVector3D color;
//    QVector2D texcoord;
};

class Mesh  : public Object
{
public:
    Mesh();
    Mesh(const std::shared_ptr<Material>&);
    ~Mesh();
    void setPositions(const QVector<QVector3D>&);
    void setVertices(const QVector<Vertex>&);
    void setIndices(const QVector<int>&);
    QVector<QVector3D> getPositions(){return positions;}
    QVector<Vertex> getVertices(){return vertices;}
    QVector<int> getIndices(){return indices;}
    QVector<std::shared_ptr<Triangle>> getTriangles(){return triangles;}
    void createTriangles();
    void appengTriangle(const std::shared_ptr<Triangle>&);

    bool intersect(const Ray &ray, Intersection &intersection) override;

private:
    QVector<QVector3D> positions;
    QVector<Vertex> vertices;
    QVector<int> indices;
//    QVector<Triangle> triangles;
    QVector<std::shared_ptr<Triangle>> triangles;
};

class AreaLight : public Mesh{
public:
    AreaLight(){}
    AreaLight(float size, float height, float intensity);

    QVector3D samplePoint();

    float areaPDF();

    QVector3D normal();

private:
    float size;
    float height;
    QVector3D edge1;
    QVector3D edge2;
    QVector3D origin;
};


Mesh importFbx(const char*, const float scale = 1.0f, const QVector3D& offset = QVector3D(0, 0, 0));


#endif // MESH_H

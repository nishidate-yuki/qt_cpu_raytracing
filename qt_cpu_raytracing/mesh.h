#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QtDebug>
#include <iostream>
#include "fbxsdk.h"

struct Vertex{
    QVector3D position;
    QVector3D normal;
//    QVector3D color;
//    QVector2D texcoord;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();
    void setPositions(const QVector<QVector3D>&);
    void setVertices(const QVector<Vertex>&);
    void setIndices(const QVector<int>&);
    QVector<QVector3D> getPositions(){return positions;}
    QVector<Vertex> getVertices(){return vertices;}
    QVector<int> getIndices(){return indices;}

private:
    QVector<QVector3D> positions;
    QVector<Vertex> vertices;
    QVector<int> indices;
};


Mesh importFbx(const char*);


#endif // MESH_H

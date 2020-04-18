#ifndef UTILS_H
#define UTILS_H

//#include <stdlib.h>
#include <QVector3D>

float frand();

float dot(const QVector3D&, const QVector3D&);

QVector3D cross(const QVector3D&, const QVector3D&);

QVector3D normalize(const QVector3D&);

void orthonormalize(const QVector3D&, QVector3D&, QVector3D&);

QVector3D worldToLocal(const QVector3D&, const QVector3D&, const QVector3D&, const QVector3D&);

QVector3D localToWorld(const QVector3D&, const QVector3D&, const QVector3D&, const QVector3D&);
#endif // UTILS_H
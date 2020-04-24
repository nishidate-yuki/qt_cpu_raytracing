#ifndef UTILS_H
#define UTILS_H

#include <QVector3D>
#include <QVector>
#include <tuple>
#include <QColor>
#include <QEventLoop>
#include <QTimer>

float frand();

float dot(const QVector3D&, const QVector3D&);

QVector3D cross(const QVector3D&, const QVector3D&);

QVector3D normalize(const QVector3D&);

void gammaCorrection(QVector<QVector<QVector3D>> &);
QVector3D gammaCorrection(QVector3D &);

QColor colorFromVector(QVector3D &);

QVector<QVector<QVector3D>> createImage(const int&, const int&);

//void sleep(int);

#endif // UTILS_H

#include "coordinateconverter.h"

CoordinateConverter::CoordinateConverter(const QVector3D &vector)
{
    // 入力されたvectorはy方向なのでvに入れる
    v = normalize(vector);
    if(qAbs(v.x()) > 0.9)
        u = QVector3D(0, 1, 0); // v2 = y
    else
        u = QVector3D(1, 0, 0); // v2 = x

    u = normalize(u - dot(u, v)*v);
    w = cross(u, v);
}

QVector3D CoordinateConverter::toLocal(const QVector3D &vector)
{
    return QVector3D(dot(vector, u), dot(vector, v), dot(vector, w));
}

QVector3D CoordinateConverter::toWorld(const QVector3D &vector)
{
    QVector3D a(u.x(), v.x(), w.x());
    QVector3D b(u.y(), v.y(), w.y());
    QVector3D c(u.z(), v.z(), w.z());
    return QVector3D(dot(vector, a), dot(vector, b), dot(vector, c));
}

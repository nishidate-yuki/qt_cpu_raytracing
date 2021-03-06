#ifndef COODINATECONVERTER_H
#define COODINATECONVERTER_H

#include <QVector3D>
#include "utils.h"

class CoordinateConverter
{
public:
    CoordinateConverter(const QVector3D&);

    QVector3D toLocal(const QVector3D&);

    QVector3D toWorld(const QVector3D&);

private:
    QVector3D u;
    QVector3D v;
    QVector3D w;
};

#endif // COODINATECONVERTER_H

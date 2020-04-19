#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>
#include <QtMath>
#include "utils.h"

QVector3D reflect(const QVector3D&, const QVector3D&);

class Material
{
public:
    virtual QVector3D sample(const QVector3D& direction, float& pdf, int& depth) const = 0;
    virtual QVector3D getWeight(const QVector3D& direction, float& theta) const = 0;

    QVector3D getScatter() {return scatter;}
    QVector3D getEmission() {return emission;}

protected:
    QVector3D scatter;
    QVector3D emission;
};

class Diffuse : public Material
{
public:
    Diffuse();
    Diffuse(const QVector3D&);
    ~Diffuse() {}

    QVector3D sample(const QVector3D& direction, float& pdf, int& depth) const override;
    QVector3D getWeight(const QVector3D& direction, float& theta) const override;
};

class Mirror : public Material
{
public:
    Mirror();
    ~Mirror() {}

    QVector3D sample(const QVector3D& direction, float& pdf, int& depth) const override;
    QVector3D getWeight(const QVector3D& direction, float& theta) const override;
};

class Light : public Material
{
public:
    Light();
    ~Light() {}

    QVector3D sample(const QVector3D& direction, float& pdf, int& depth) const override;
    QVector3D getWeight(const QVector3D& direction, float& theta) const override;
};

#endif // MATERIAL_H

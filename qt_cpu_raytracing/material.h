#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>
#include <QtMath>
#include "utils.h"

float absCosTheta(const QVector3D&);

QVector3D reflect(const QVector3D&, const QVector3D&);

float fresnel(const QVector3D&, const QVector3D&, float, float);

class Material
{
public:
    // dir, weight を返す
    virtual std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) = 0;
    virtual QVector3D evaluate(const QVector3D&, const QVector3D&) = 0;

    QVector3D getColor() {return color;}
    QVector3D getEmission() {return emission;}

protected:
    QVector3D color;
    QVector3D emission;
};

class Diffuse : public Material
{
public:
    Diffuse();
    Diffuse(const QVector3D&);
    ~Diffuse() {}

    std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) override;
    QVector3D evaluate(const QVector3D&, const QVector3D&) override;
};

class Mirror : public Material
{
public:
    Mirror();
    ~Mirror() {}

    std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) override;
    QVector3D evaluate(const QVector3D&, const QVector3D&) override{return QVector3D();}
};

class Light : public Material
{
public:
    Light();
    Light(const QVector3D&);
    Light(const float);
    ~Light() {}

    std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) override;
    QVector3D evaluate(const QVector3D&, const QVector3D&) override{return QVector3D();}
};

class Glass : public Material
{
public:
    Glass();
    ~Glass() {}

    std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) override;
    QVector3D evaluate(const QVector3D&, const QVector3D&) override{return QVector3D();}

    float ior;
};

#endif // MATERIAL_H

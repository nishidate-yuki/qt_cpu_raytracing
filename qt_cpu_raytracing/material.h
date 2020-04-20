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
//    virtual std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) = 0;
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

    std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) override;
    QVector3D getWeight(const QVector3D& direction, float& theta) const override;
};

class Mirror : public Material
{
public:
    Mirror();
    ~Mirror() {}

    std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) override;
    QVector3D getWeight(const QVector3D& direction, float& theta) const override;
};

//class Glass : public Material
//{
//public:
//    Glass();
//    ~Glass() {}

//    QVector3D sample(const QVector3D& direction, float& pdf, int& depth) override;
//    QVector3D getWeight(const QVector3D& direction, float& theta) const override;

//    float ior;
//    float fresnelReflectance;
//    int res;
//};

class Light : public Material
{
public:
    Light();
    Light(const QVector3D&);
    ~Light() {}

    std::tuple<QVector3D, QVector3D> sample(const QVector3D& direction, int& depth) override;
    QVector3D getWeight(const QVector3D& direction, float& theta) const override;
};

#endif // MATERIAL_H

#ifndef MATERIAL_H
#define MATERIAL_H

//struct Material{
//    QVector3D scatter;
//    QVector3D emission;
//    MaterialType materialType;

//    Material() :
//        scatter(1.0, 1.0, 1.0), emission(0, 0, 0),
//        materialType(DIFFUSE) {}
//};


//enum MaterialType {
//    DIFFUSE,	// 完全拡散面
//    SPECULAR,	// 完全鏡面
//    REFRACTION,	// 完全なガラス
//    LIGHT,      // ライト
//};

#include <QVector3D>
#include <QtMath>

#include "utils.h"

QVector3D reflect(const QVector3D&, const QVector3D&);

class Material
{
public:
//    Material();
//    virtual ~Material();

    virtual QVector3D sample(const QVector3D& direction, float& pdf, int& depth) const = 0;
    virtual QVector3D getWeight(const QVector3D& direction, float& theta) const = 0;

    QVector3D scatter;
    QVector3D emission;
};

class Diffuse : public Material
{
public:
    Diffuse();
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
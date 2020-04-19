#include "material.h"

QVector3D reflect(const QVector3D& v, const QVector3D& n) {
    return -v + 2*dot(v, n)*n;
}

// Diffuse
Diffuse::Diffuse()
{
    scatter = QVector3D(1.0, 1.0, 1.0);
    emission = QVector3D(0, 0, 0);
}

Diffuse::Diffuse(const QVector3D& color)
{
    scatter = color;
}

QVector3D Diffuse::sample(const QVector3D &direction, float &pdf, int& depth) const
{
    // この関数の中はすべてローカル座標系と考えていい
    depth++;
    // importance sampling
    float phi = 2 * M_PI * frand();
    float y = sqrt(frand());
    float d = sqrt(1 - y*y);
    float x = d * cos(phi);
    float z = d * sin(phi);

    return QVector3D(x, y, z);
}

QVector3D Diffuse::getWeight(const QVector3D &direction, float &theta) const
{
    // レンダリング方程式の第2項の中の、Li以外の部分を返す
    //    BRDF * cosθ / pdf
    // = (ρ/π) * cosθ / (π/cosθ)
    // = ρ
    // ρ : 反射率。ここではscatter
    return scatter;
}

// Mirror
Mirror::Mirror()
{
    scatter = QVector3D(1, 1, 1);
    emission = QVector3D(0, 0, 0);
}

QVector3D Mirror::sample(const QVector3D &direction, float &pdf, int& depth) const
{
    depth++;
    return reflect(direction, QVector3D(0, 1, 0));
}

QVector3D Mirror::getWeight(const QVector3D &direction, float &theta) const
{
    // レンダリング方程式の第2項の中の、Li以外の部分を返す
    //    BRDF * cosθ / pdf
    // = (1*ρ) * cosθ / (π/cosθ)
    // = ρ
    // ρ : 反射率。ここではscatter
    return scatter;
}

// Light
Light::Light()
{
    scatter = QVector3D(0, 0, 0);
    emission = QVector3D(1, 1, 1);
}

QVector3D Light::sample(const QVector3D &direction, float &pdf, int& depth) const
{
    depth = INT_MAX;
    return QVector3D(0, 0, 0);
}

QVector3D Light::getWeight(const QVector3D &direction, float &theta) const
{
    return QVector3D(0, 0, 0);
}

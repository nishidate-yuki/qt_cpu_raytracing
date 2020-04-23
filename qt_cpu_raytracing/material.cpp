#include "material.h"

float absCosTheta(const QVector3D& v) {
    return abs(v.normalized().y());
}

float theta(const QVector3D& v){
    return acos(v.normalized().y());
}

QVector3D reflect(const QVector3D& v, const QVector3D& n) {
    return -v + 2*dot(v, n)*n;
}

// フレネル反射率
// Schlickの近似式を用いる
float fresnel(const QVector3D& v, const QVector3D& n, float n1, float n2) {
  float f0 = pow((n1 - n2)/(n1 + n2), 2.0);
  float cos = absCosTheta(v);
  return f0 + (1 - f0) * pow(1 - cos, 5.0);
}

// 全反射
bool totalReflect(const QVector3D& v, float n1, float n2){
    // snellの法則から
    // n1 * sin(θ1) = n2 * sin(90°)
    //      sin(θ1) = n2 / n1

    float cos = absCosTheta(v);
    float sin = sqrt(qMax(1 - cos*cos, 0.0f));
    float alpha = n1/n2 * sin;

    return alpha * alpha > 1.0f;
}

QVector3D refract(const QVector3D& v, const QVector3D& n, float n1, float n2) {
    float cos = absCosTheta(v);
    float sin = sqrt(qMax(1 - cos*cos, 0.0f));
    float alpha = n1/n2 * sin;

    // 屈折ベクトル
    return n1/n2 * (-v + dot(v, n)*n) - sqrt(1 - alpha*alpha)*n;
}

// Diffuse
Diffuse::Diffuse()
{
    color = QVector3D(0.9, 0.9, 0.9);
    emission = QVector3D(0, 0, 0);
}

Diffuse::Diffuse(const QVector3D& color)
{
    this->color = color;
}

std::tuple<QVector3D, QVector3D> Diffuse::sample(const QVector3D &direction, int &depth)
{
    depth++;
    // importance sampling
    float phi = 2 * M_PI * frand();
    float y = sqrt(frand());
    float d = sqrt(1 - y*y);
    float x = d * cos(phi);
    float z = d * sin(phi);

    return {{x, y, z}, color};
}

// Mirror
Mirror::Mirror()
{
    color = QVector3D(1, 1, 1);
    emission = QVector3D(0, 0, 0);
}

std::tuple<QVector3D, QVector3D> Mirror::sample(const QVector3D &direction, int &depth)
{
    depth++;
    return {reflect(direction, QVector3D(0, 1, 0)), color};
}

// Light
Light::Light()
{
    color = QVector3D(0, 0, 0);
    emission = QVector3D(1, 1, 1);
}

Light::Light(const QVector3D &color)
{
    emission = color;
}

std::tuple<QVector3D, QVector3D> Light::sample(const QVector3D &direction, int &depth)
{
    depth = INT_MAX;
    return {{0, 0, 0}, {0, 0, 0}};
}

// Glass
Glass::Glass()
    : ior(1.5)
{
    color = {1, 1, 1};
    emission = {0, 0, 0};
}

std::tuple<QVector3D, QVector3D> Glass::sample(const QVector3D& direction, int& depth)
{
    depth++;

    QVector3D normal(0, 1, 0);
    const bool isEntering = dot(direction, normal) > 0;

    // 屈折率
    float n1 = 1.0; // 入射側
    float n2 = ior; // 出射側

    // 物体から出ていく方向であればnormalを反転
    // 屈折率も逆にする
    if(!isEntering){
        normal = {0, -1, 0};
        n1 = ior;
        n2 = 1.0;
    }

    // 全反射
    if(totalReflect(direction, n1, n2)){
        auto reflectDir = reflect(direction, {0, 1, 0});
        auto weight = color;
        return {reflectDir, weight};
    }

    // フレネル反射率
    float fresnelReflectance = fresnel(direction, normal, n1, n2);

    // 反射
    if(frand() < fresnelReflectance){
        auto reflectDir = reflect(direction, normal);
        auto weight = color;
        return {reflectDir, weight};
    }
    // 屈折
    else{
        auto reflectDir = refract(direction, normal, n1, n2);
        auto weight = color;
        // 放射輝度(ここではweight)に相対輝度の2乗を掛ける
        weight *= pow(n1/n2, 2.0);
        return {reflectDir, weight};
    }
}

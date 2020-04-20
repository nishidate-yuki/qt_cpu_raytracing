#include "material.h"

float absCosTheta(const QVector3D& v) {
    return abs(v.normalized().y());
}

//float sinTheta(const QVector3D& v) {
//    QVector3D u = v.normalized();
//    u[1] = 0; // x, z平面に投影
//    return u.length();
//}

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
//    float criticalAngle = asin(n2 / n1);
//    return theta(v) > criticalAngle;

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

//bool refract(const QVector3D& v, QVector3D& r, const QVector3D& n, float n1, float n2) {
//  float cos = absCosTheta(v);
//  float sin = sqrt(qMax(1 - cos*cos, 0.0f));
//  float alpha = n1/n2 * sin;

//  //全反射
//  if(alpha*alpha > 1.0) return false;

//  //屈折ベクトル
//  r = n1/n2 * (-v + dot(v, n)*n) - sqrt(1 - alpha*alpha)*n;

//  return true;
//}



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

QVector3D Diffuse::sample(const QVector3D &direction, float &pdf, int& depth)
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

std::tuple<QVector3D, QVector3D> Diffuse::sample(const QVector3D &direction, int &depth)
{
    depth++;
    // importance sampling
    float phi = 2 * M_PI * frand();
    float y = sqrt(frand());
    float d = sqrt(1 - y*y);
    float x = d * cos(phi);
    float z = d * sin(phi);

    return {{x, y, z}, scatter};
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

QVector3D Mirror::sample(const QVector3D &direction, float &pdf, int& depth)
{
    depth++;
    return reflect(direction, QVector3D(0, 1, 0));
}

std::tuple<QVector3D, QVector3D> Mirror::sample(const QVector3D &direction, int &depth)
{
    depth++;
    return {reflect(direction, QVector3D(0, 1, 0)), scatter};
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

Light::Light(const QVector3D &color)
{
    emission = color;
}

QVector3D Light::sample(const QVector3D &direction, float &pdf, int& depth)
{
    depth = INT_MAX;
    return QVector3D(0, 0, 0);
}

std::tuple<QVector3D, QVector3D> Light::sample(const QVector3D &direction, int &depth)
{
    depth = INT_MAX;
    return {{0, 0, 0}, {0, 0, 0}};
}

QVector3D Light::getWeight(const QVector3D &direction, float &theta) const
{
    return QVector3D(0, 0, 0);
}

//// Glass
//Glass::Glass()
//    : ior(1.5)
//{
//    scatter = {1, 1, 1};
//    emission = {0, 0, 0};
//}

//QVector3D Glass::sample(const QVector3D &direction, float &pdf, int &depth)
//{
//    QVector3D normal(0, 1, 0);
//    const bool isEntering = dot(direction, normal) > 0;

//    // 屈折率
//    float n1 = 1.0; // 入射側
//    float n2 = ior; // 出射側

//    // 物体から出ていく方向であればnormalを反転
//    // 屈折率も逆にする
//    if(!isEntering){
//        normal = {0, -1, 0};
//        n1 = ior;
//        n2 = 1.0;
//    }

//    // 全反射
//    if(totalReflect(direction, n1, n2)){
//        res = 0;
//        return reflect(direction, {0, 1, 0});
//    }

//    // フレネル反射率
//    fresnelReflectance = fresnel(direction, normal, n1, n2);

//    // 反射
//    if(frand() < fresnelReflectance){
//        res = 1;
//        return reflect(direction, normal);
//    }
//    // 屈折
//    else{
//        res =2;
//        return refract(direction, normal, n1, n2);
//    }

////    // 反射
////    if(frand() < fr) {
////        wi = reflect(wo, normal);
////        pdf = fr;
////        return fr/absCosTheta(wi) * Vec3(1);
////    }
////    //屈折
////    else {
////        if(refract(wo, wi, normal, n1, n2)) {
////            pdf = 1 - fr;
////            return std::pow(n1/n2, 2.0) * (1 - fr)/absCosTheta(wi) * Vec3(1);
////        }
////        //全反射
////        else {
////            wi = reflect(wo, normal);
////            pdf = 1 - fr;
////            return (1 - fr)/absCosTheta(wi) * Vec3(1);
////        }
////    }

//}

//QVector3D Glass::getWeight(const QVector3D &direction, float &theta) const
//{
//    QVector3D normal(0, 1, 0);
//    const bool isEntering = dot(direction, normal) > 0;

//    // 屈折率
//    float n1 = 1.0; // 入射側
//    float n2 = ior; // 出射側

//    // 物体から出ていく方向であればnormalを反転
//    // 屈折率も逆にする
//    if(!isEntering){
//        normal = {0, -1, 0};
//        n1 = ior;
//        n2 = 1.0;
//    }

//    if(res == 0){
//        return scatter;
////        return (1 - fresnelReflectance)/absCosTheta(direction) * QVector3D{1, 1, 1};
//    }else if(res == 1){
////        return scatter / fresnelReflectance;
//        return scatter * fresnelReflectance;
////        return fresnelReflectance/absCosTheta(direction) * QVector3D{1, 1, 1};
//    }else{
////        return scatter / (1.0 - fresnelReflectance);
//        return scatter * (1.0 - fresnelReflectance);
////        return std::pow(n1/n2, 2.0) * (1 - fresnelReflectance)/absCosTheta(direction) * QVector3D{1, 1, 1};
//    }

//}

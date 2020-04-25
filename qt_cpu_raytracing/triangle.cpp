#include "triangle.h"

Triangle::Triangle()
    : v0(0, 0, 0), v1(0, 0, 0), v2(0, 0, 0)
{

}

Triangle::Triangle(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2)
    : v0(v0), v1(v1), v2(v2)
{

}

Triangle::~Triangle()
{
//    qDebug() << "Triangle deleted";
}

bool Triangle::intersect(const Ray &ray, Intersection &intersection)
{
    // Tomas Mollerの交差判定

    const QVector3D edge1 = v1 - v0;
    const QVector3D edge2 = v2 - v0;

    // α : d × e2
    QVector3D alpha = cross(ray.direction, edge2);

    // det : 行列式(determinant)
    // detが0に近いときはfalse
    float det = dot(edge1, alpha);
    if(abs(det) < FLT_MIN) { return false; }

    // invDet : inverse determinant
    // クラメールの公式の分母 (det A)
    float invDet = 1.0f / det;

    // 方程式の右辺を1文字にまとめる
    // r : o - v0
    QVector3D r = ray.origin - v0;

    // u を計算し、[0, 1]であるかチェック
    // u : edge1方向の係数
    float u = dot(alpha, r) * invDet;
    if (u < 0.0f || 1.0f < u) { return false; }

    // β : (o - v0) × e1
    QVector3D beta = cross(r, edge1);

    // v を計算し、[0, 1-u]であるかチェック
    float v = dot(ray.direction, beta) * invDet;
    if (v < 0.0f || 1.0f < u + v) { return false; }

    // rayの逆方向で交差していたらfalse
    float t = dot(edge2, beta) * invDet;
    if (t < 0.0f) { return false; }

    // Intersectionの情報を計算する
    intersection.distance = t;
    intersection.normal = normal();
    intersection.position = ray.origin + t * ray.direction;

    return true;
}

QVector3D Triangle::normal()
{
    return QVector3D::normal(v0, v1, v2);
}

QVector3D &Triangle::operator[](int i)
{
    if(i == 0){
        return v0;
    }else if(i == 2){
        return v1;
    }else{
        return v2;
    }
}

const QVector3D &Triangle::operator[](int i) const
{
    if(i == 0){
        return v0;
    }else if(i == 2){
        return v1;
    }else{
        return v2;
    }
}

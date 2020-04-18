#include "utils.h"


float frand(){
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float dot(const QVector3D& a, const QVector3D& b){
    return QVector3D::dotProduct(a, b);
}

QVector3D cross(const QVector3D& a, const QVector3D& b){
    return QVector3D::crossProduct(a, b);
}

QVector3D normalize(const QVector3D& v){
    return v.normalized();
}

// 正規直交化
// v1 -> (v1, v2, v3)
void orthonormalize(const QVector3D& v1, QVector3D& v2, QVector3D& v3) {
    if(qAbs(v1.x()) > 0.9)
        v2 = QVector3D(0, 1, 0); // v2 = y
    else
        v2 = QVector3D(1, 0, 0); // v2 = x

    v2 = normalize(v2 - dot(v1, v2)*v1);
    v3 = cross(v1, v2);
}

//ワールド座標系からローカル座標系に変換する
QVector3D worldToLocal(const QVector3D& v, const QVector3D& s, const QVector3D& n, const QVector3D& t) {
  return QVector3D(dot(v, s), dot(v, n), dot(v, t));
}

//ローカル座標系からワールド座標系に変換する
QVector3D localToWorld(const QVector3D& v, const QVector3D& s, const QVector3D& n, const QVector3D& t) {
  QVector3D a(s.x(), n.x(), t.x());
  QVector3D b(s.y(), n.y(), t.y());
  QVector3D c(s.z(), n.z(), t.z());
  return QVector3D(dot(v, a), dot(v, b), dot(v, c));
}

// ガンマ補正
void gammaCorrection(QVector<QVector<QVector3D>>& fImage) {
    int width = fImage[0].size();
    int height = fImage.size();
    for(int w = 0; w < width; w++) {
        for(int h = 0; h < height; h++) {
            QVector3D c = fImage[h][w];
            fImage[h][w] = QVector3D(pow(c.x(), 1/2.2), pow(c.y(), 1/2.2), pow(c.z(), 1/2.2));
        }
    }
//    for(int i = 0; i < image.width(); i++) {
//        for(int j = 0; j < image.height(); j++) {
//            QColor c = image.pixelColor(i, j);
//            this->setPixel(i, j, Vec3(std::pow(c.x, 1/2.2), std::pow(c.y, 1/2.2), std::pow(c.z, 1/2.2)));
//        }
//    }
};


#define STB_IMAGE_IMPLEMENTATION
#include "sky.h"

UniformSky::UniformSky(const QVector3D &_color) : color(_color) {}

QVector3D UniformSky::getRadiance(const Ray &ray) const {
    return color;
}

QVector3D SimpleSky::getRadiance(const Ray &ray) const {
    double t = (ray.direction.y() + 1)/2;
    return (1 - t)*QVector3D(1, 1, 1) + t*QVector3D(0.7, 0.8, 1);
}

IBL::IBL(const char* filename) {
    int n;
    hdr_image = stbi_loadf(filename, &width, &height, &n, 0);
}

IBL::~IBL() {
    stbi_image_free(hdr_image);
}

QVector3D IBL::getRadiance(const Ray &ray) const {
    //方向を球面座標系に変換
    double theta = acos(ray.direction.y());
    double phi = atan2(ray.direction.z(), ray.direction.x());
    if(phi < 0) phi += 2*M_PI;

    //(i, j)を計算
    int i = phi/(2*M_PI) * width;
    int j = theta/M_PI * height;

    //(i, j)の配列内におけるインデックスを計算
    int index = 3*i + 3*width*j;

    return QVector3D(hdr_image[index], hdr_image[index+1], hdr_image[index+2]);
}

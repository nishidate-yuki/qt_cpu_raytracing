#ifndef SKY_H
#define SKY_H

#include "stb_image.h"
#include <QtMath>
#include "ray.h"

class Sky {
  public:
    // 放射輝度の値を返す
    virtual QVector3D getRadiance(const Ray& ray) const = 0;
};

// 単色のSky
class UniformSky : public Sky {
  public:
    QVector3D color;

    UniformSky(const QVector3D& _color);

    QVector3D getRadiance(const Ray& ray) const;
};

// 大気色のSky
class SimpleSky : public Sky {
  public:

    QVector3D getRadiance(const Ray& ray) const;
};

// HDRI
class IBL : public Sky {
  public:
    int width;
    int height;
    float* hdr_image;

    IBL(const char* filename);
    ~IBL();

    QVector3D getRadiance(const Ray& ray) const;
};

#endif // SKY_H

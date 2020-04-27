#include "utils.h"


float frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float dot(const QVector3D& a, const QVector3D& b)
{
    return QVector3D::dotProduct(a, b);
}

QVector3D cross(const QVector3D& a, const QVector3D& b)
{
    return QVector3D::crossProduct(a, b);
}

QVector3D normalize(const QVector3D& v)
{
    return v.normalized();
}

// ガンマ補正
void gammaCorrection(QVector<QVector<QVector3D>> &fImage)
{
    int width = fImage[0].size();
    int height = fImage.size();

    for(int w = 0; w < width; w++) {
        for(int h = 0; h < height; h++) {
            QVector3D c = fImage[h][w];
            fImage[h][w] = QVector3D(pow(c.x(), 1/2.2), pow(c.y(), 1/2.2), pow(c.z(), 1/2.2));
        }
    }
}
QVector3D gammaCorrection(QVector3D &fColor)
{
    return QVector3D(pow(fColor.x(), 1/2.2), pow(fColor.y(), 1/2.2), pow(fColor.z(), 1/2.2));
}

// 画像初期化
QVector<QVector<QVector3D>> createImage(const int &width, const int &height)
{
    // Setup Image
    QVector<QVector<QVector3D>> fImage(height);
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
//            QVector3D color(float(h)/height, float(w)/width, 0);
            QVector3D color(0, 0, 0);
            fImage[h].append(color);
        }
    }
    return fImage;
}

QColor colorFromVector(QVector3D &fColor)
{
    QColor color;
    color.setRgbF(qBound(0.0f, fColor.x(), 1.0f),
                  qBound(0.0f, fColor.y(), 1.0f),
                  qBound(0.0f, fColor.z(), 1.0f));
    return color;
}

//void sleep(int msec)
//{
//    QEventLoop loop;
//    QTimer::singleShot(msec, &loop, SLOT(quit()));
//    loop.exec();
//}

float length(const QVector3D &v)
{
    return v.length();
}

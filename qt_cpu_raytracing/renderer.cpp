#include "renderer.h"

constexpr int DEPTH = 32;

Renderer::Renderer(int width, int height)
    : width(width), height(height)
{
}

void Renderer::render() {
    qDebug() << "start rendering";

    QImage image(width, height, QImage::Format_RGB32);

    const double screenWidth = 15.0 * width / height;
    const double screenHeight= 15.0;
    QVector3D cameraPosition(0, 0, 25);

    // materials
    auto diffuse = std::make_shared<Diffuse>();
    auto redDiffuse = std::make_shared<Diffuse>(QVector3D(0.9, 0.1, 0.1));
    auto greenDiffuse = std::make_shared<Diffuse>(QVector3D(0.1, 0.9, 0.1));
    auto mirror = std::make_shared<Mirror>();
    auto glass = std::make_shared<Glass>();
    auto light = std::make_shared<Light>(5);
    auto black = std::make_shared<Light>(0);

    // cornell box
    float boxSize = 16;
    float sphereRad = 2;
    QVector<std::shared_ptr<Object>> cornellBox;
    cornellBox << std::make_shared<Sphere>(QVector3D(0, -(10000+boxSize/2), 0), 10000, diffuse); // floor
    cornellBox << std::make_shared<Sphere>(QVector3D(0, (10000+boxSize/2), 0), 10000, diffuse);  // ceiling
    cornellBox << std::make_shared<Sphere>(QVector3D((10000+boxSize/2), 0, 0), 10000, greenDiffuse);  // right
    cornellBox << std::make_shared<Sphere>(QVector3D(-(10000+boxSize/2), 0, 0), 10000, redDiffuse); // left
    cornellBox << std::make_shared<Sphere>(QVector3D(0, 0, -(10000+boxSize/2)), 10000, diffuse); // front
    cornellBox << std::make_shared<Sphere>(QVector3D(0, 0, 10030), 10000, black); // back
    Mesh bunnyHigh = importFbx("E:/3D Objects/cornell_box/bunny.fbx");
    cornellBox << std::make_shared<BVH>(bunnyHigh);
    // area light
    areaLight = AreaLight(4, boxSize/2 - 0.01f, 20);
    cornellBox << std::make_shared<AreaLight>(areaLight);

#pragma omp parallel for schedule(dynamic, 1)
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            QVector3D fColor(0, 0, 0);
            for (int n=0; n<num_samples; n++) {
                // screenPositionを計算
                QVector3D screenPosition;
                screenPosition[0] = (w+frand())/width * screenWidth - screenWidth/2.0;
                screenPosition[1] = (height-(h+frand()))/height * screenHeight - screenHeight/2.0;
                screenPosition[2] = cameraPosition[2] - 18;

                // rayを生成
                Ray ray(cameraPosition);
                ray.direction = (screenPosition - cameraPosition).normalized();

                // radianceを計算
                int depth = 0;
                fColor += radiance(ray, cornellBox, depth);
            }
            fColor = fColor/num_samples;
            fColor = gammaCorrection(fColor);

            image.setPixelColor(w, h, colorFromVector(fColor));
        }
        if(omp_get_thread_num() == 0) {
            qDebug() << int(double(h)/height * 100)  << "%";
        }
    }

    qDebug() << "finish rendering";

    emit finished(image);
}

QVector3D Renderer::radiance(Ray &ray, QVector<std::shared_ptr<Object>> &scene, int &depth)
{
    // シーンとの交差判定
    Intersection intersection;
    if(!intersectScene(ray, scene, intersection)) return {0, 0, 0};
    std::shared_ptr<Object> obj = scene[intersection.objectIndex];

    // first rayの場合のみ、光源に当たったら寄与を蓄積する
    auto material = obj->material.get();
    if(depth == 0){
        if(typeid(*material) == typeid(Light)){
            return obj->material->getEmission();
        }
    }

    // ローカル座標系を作る
    CoordinateConverter converter(intersection.normal);

    // world座標 -> local座標
    QVector3D localDirection = converter.toLocal(-ray.direction);

    // rayの方向とweightを計算する
    // weight = BRDF * cosθ / pdf
    auto [nextDirection, weight] = obj->material->sample(localDirection, depth);

    // ray更新
    ray.direction = converter.toWorld(nextDirection);
    ray.origin = intersection.position + ray.direction * 0.002f;

    // Emittion
    QVector3D emittion;
    QVector3D inRadiance;

    // Next Event Estimation
    // MirrorとGlass以外でNEE実行
    if(typeid(*material) != typeid(Mirror) && typeid(*material) != typeid(Glass)){
        // shadow rayを計算
        QVector3D lightPoint = areaLight.samplePoint();
        QVector3D lightNormal = areaLight.normal();
        QVector3D shadowDir = normalize(lightPoint - intersection.position);
        QVector3D shadowOri = intersection.position + shadowDir*0.002f;
        Ray shadowRay(shadowOri, shadowDir);

        // shadow rayを飛ばす
        Intersection shadowInter;
        if(intersectScene(shadowRay, scene, shadowInter)){
            // ヒットしたオブジェクトのクラスを調べる
            // shared_ptrの中身を取り出す
            auto shadowObj = scene[shadowInter.objectIndex].get();
            // 光源にヒットした場合
            if(typeid(*shadowObj) == typeid(AreaLight)){
                QVector3D fs = obj->material->evaluate(shadowDir, -ray.direction);
                float areaPDF = areaLight.areaPDF();
                float jacobian = abs(dot(intersection.normal, shadowDir))
                                 * abs(dot(lightNormal, shadowDir))
                                 / pow(length(lightPoint - intersection.position), 2);
                emittion = fs * areaLight.material->getEmission() * jacobian / areaPDF;
            }
        }
        // 再帰でradiance取得
        if(depth > DEPTH) return emittion;

        inRadiance = radiance(ray, scene, depth);
    }else{
        if(depth > DEPTH) return obj->material->getEmission();
        inRadiance = radianceIBL(ray, scene, depth);
    }

    // 最終的なレンダリング方程式
    // Lo = Le + (BRDF * Li * cosθ)/pdf
    return emittion + weight * inRadiance;
}


QVector3D Renderer::radianceIBL(Ray &ray, QVector<std::shared_ptr<Object>> &scene, int &depth)
{
    static IBL sky("E:/Pictures/Textures/_HDRI/4k/rural_landscape_4k.hdr");

    // シーンとの交差判定
    Intersection intersection;
    if(!intersectScene(ray, scene, intersection)) return sky.getRadiance(ray);
    std::shared_ptr<Object> obj = scene[intersection.objectIndex];

    // ローカル座標系を作る
    CoordinateConverter converter(intersection.normal);

    // world座標 -> local座標
    QVector3D localDirection = converter.toLocal(-ray.direction);

    // rayの方向とweightを計算する
    // weight = BRDF * cosθ / pdf
    auto [nextDirection, weight] = obj->material->sample(localDirection, depth);

    // ray更新
    ray.direction = converter.toWorld(nextDirection);
    ray.origin = intersection.position + ray.direction * 0.002f;

    // 再帰でradiance取得
    if(depth > DEPTH) return obj->material->getEmission();
    QVector3D inRandiance = radianceIBL(ray, scene, depth);

    // Emittion
    QVector3D emittion = obj->material->getEmission();

    // 最終的なレンダリング方程式
    // Lo = Le + (BRDF * Li * cosθ)/pdf
    QVector3D rad = emittion + weight * inRandiance;

    return rad;
}

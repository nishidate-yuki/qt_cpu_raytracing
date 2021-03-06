#include "bvh.h"

BoundingBox createBB(const std::shared_ptr<Triangle> &tri)
{
    BoundingBox bbox;
    for (int axis=0; axis<3; axis++) {
        bbox.min[axis] = std::min({(*tri)[0][axis], (*tri)[1][axis], (*tri)[2][axis]});
        bbox.max[axis] = std::max({(*tri)[0][axis], (*tri)[1][axis], (*tri)[2][axis]});
    }
    return bbox;
}

QVector3D calcCenter(const BoundingBox &bbox)
{
    QVector3D center;
    center[0] = (bbox.min[0] + bbox.max[0]) / 2.0;
    center[1] = (bbox.min[1] + bbox.max[1]) / 2.0;
    center[2] = (bbox.min[2] + bbox.max[2]) / 2.0;
    return center;
}

// BB の表面積計算
float calcSurfaceArea(const BoundingBox &bbox) {
    float dx = bbox.max[0] - bbox.min[0];
    float dy = bbox.max[1] - bbox.min[1];
    float dz = bbox.max[2] - bbox.min[2];
    return 2*(dx*dy + dx*dz * dy*dz);
}

// 空の BB を作成
void emptyBB(BoundingBox &bbox) {
    bbox.min = { FLT_MAX, FLT_MAX, FLT_MAX }; //最小座標を大きく初期化
    bbox.max = { -FLT_MAX, -FLT_MAX, -FLT_MAX }; //最大座標を小さく初期化
}

// 2つのAABBをマージ
// 小さいBB二つ -> それを囲む大きいBBをつくる
BoundingBox mergeBB(const BoundingBox &bbox1, const BoundingBox &bbox2) {
    BoundingBox result;
    for (int axis=0; axis<3; axis++) {
        result.min[axis] = std::min(bbox1.min[axis], bbox2.min[axis]);
        result.max[axis] = std::max(bbox1.max[axis], bbox2.max[axis]);
    }
    return result;
}

// ノードを polygons を含む葉ノードにする
void makeLeaf(std::shared_ptr<BVHnode> &node, std::shared_ptr<Triangle> &triangle)
{
    if(node == nullptr){
        qDebug() << "node is nullptr";
        return;
    }
    // node の子は-1に初期化
    node->left = node->right = nullptr;
    // triangleをセット
    node->triangle = triangle;
}

// trianglesからBBを生成
BoundingBox createBBfromTriangles(const QVector<std::shared_ptr<Triangle>> &triangles)
{
    BoundingBox bbox;
    for(auto &t : triangles){
        bbox = mergeBB(createBB(t), bbox);
    }
    return bbox;
}

void sortByAxis(QVector<std::shared_ptr<Triangle>> &triangles, int axis)
{
    auto lessThan = [axis](auto a, auto b) {
        return calcCenter(createBB(a))[axis] < calcCenter(createBB(b))[axis];
    };
    sort(triangles.begin(), triangles.end(), lessThan);
}

void constructBVH(QVector<std::shared_ptr<Triangle>> &triangles, std::shared_ptr<BVHnode> &node)
{
    if(node == nullptr){
        qDebug() << "node is nullptr";
        return;
    }

    // Triangleが1つなら葉ノードとして終了
    if(triangles.size() == 1){
        auto triangle = triangles[0];
        makeLeaf(node, triangle);
        return;
    }

    // Trianglesで全体のBBを設定する
    // LeafはBBではなく直接Triangleと交差判定を行うため必要ない
    node->bbox = createBBfromTriangles(triangles);

    // 一番いい axis, index, cost の組み合わせ
    auto bestDivision = std::make_tuple(0, 0, FLT_MAX);

    // ここからSAHによって分割方法を決定する
    // 全体の表面積
    float rootSA = calcSurfaceArea(node->bbox);

    for (int axis=0; axis<3; axis++){
        // axisでソート
        sortByAxis(triangles, axis);

        // 左側BBsの計算
        BoundingBox leftBB;
        QVector<BoundingBox> leftBBs;
        for (int i=0; i<triangles.size()-1; i++) {
            leftBB = mergeBB(leftBB, createBB(triangles[i]));
            // 後ろに追加
            leftBBs.push_back(leftBB);
        }

        // 右側のBBs計算
        BoundingBox rightBB;
        QVector<BoundingBox> rightBBs;
        for (int i=triangles.size()-1; i>=1; i--) {
            rightBB = mergeBB(rightBB, createBB(triangles[i]));
            // 前に追加
            rightBBs.push_front(rightBB);
        }

        // best cost を探索
        for (int i=0; i<triangles.size()-1; i++){
            float leftSA = calcSurfaceArea(leftBBs[i]);
            float rightSA = calcSurfaceArea(rightBBs[i]);

            int leftSize = i+1;
            int rightSize = triangles.size() - (i+1);
            float cost = 2*1
                         + (leftSA*leftSize + rightSA*rightSize)
                               * 1 / rootSA;
            if(cost < std::get<2>(bestDivision)){
                bestDivision = std::make_tuple(axis, i, cost);
            }
        }
    }

    // 一番良い分割情報
    int bestAxis = std::get<0>(bestDivision);
    int bestSplitIndex = std::get<1>(bestDivision);
    sortByAxis(triangles, bestAxis);

    // 子ノードを作成
    // 必ず2つ以上のTriangleを含むので両方つくって良い
    node->left = std::make_shared<BVHnode>();
    node->right = std::make_shared<BVHnode>();

    // Trianglesを分割
    QVector<std::shared_ptr<Triangle>> left, right;
    for (int i=0; i<=bestSplitIndex; i++) {
        left.append(triangles[i]);
    }
    for (int i=bestSplitIndex+1; i<triangles.size(); i++) {
        right.append(triangles[i]);
    }

    constructBVH(left, node->left);
    constructBVH(right, node->right);
}

BVH::BVH(Mesh &mesh)
{
    QElapsedTimer timer;
    timer.start();

    root = std::make_shared<BVHnode>();
    auto triangles = mesh.getTriangles();
    constructBVH(triangles, root);

    qint64 elp = timer.elapsed();
    qDebug().nospace().noquote() << alignString("BVH Constructing") << elp/1000 << "." << elp%1000 << "s";
}


bool BoundingBox::intersect(const Ray &ray)
{
    float tMax =  FLT_MAX;  // AABB からレイが外に出る時刻
    float tMin = -FLT_MAX;  // AABB にレイが侵入する時刻

    for (int i=0; i<3; i++) {
        // 軸に垂直の場合
        if(abs(ray.direction[i]) < FLT_EPSILON){
            // rayの始点が領域外ならばfalse
            if(  ray.origin[i] < this->min[i]
               || this->max[i] < ray.origin[i]){
                return false;
            }else{
                continue;
            }
        }
        float t1 = (this->min[i] - ray.origin[i])/ray.direction[i];
        float t2 = (this->max[i] - ray.origin[i])/ray.direction[i];
        float tNear = std::min(t1, t2);
        float tFar = std::max(t1, t2);
        tMin = std::max(tMin, tNear);
        tMax = std::min(tMax, tFar);

        // レイが外に出る時刻と侵入する時刻が逆転している
        // => 交差していない
        if (tMin > tMax) return false;
    }
    return true;
}

bool BVHnode::isLeaf(){
    return triangle != nullptr;
}

bool BVHnode::intersect(const Ray &ray, Intersection &intersection)
{
    // BBと交差していない場合
    if(!bbox.intersect(ray))
        return false;

    // 交差して、葉である場合
    if(isLeaf()){
        // ポリゴンとの交差判定を行う
        Intersection hitpoint;
        if(triangle->intersect(ray, hitpoint)){
            if(hitpoint.distance < intersection.distance){
                intersection = hitpoint;
                return true;
            }
        }
        return false;
    }

    // 交差して、中間ノードである場合
    bool leftIntersection = left->intersect(ray, intersection);
    bool rightIntersection = right->intersect(ray, intersection);

    return leftIntersection || rightIntersection;
}

bool BVH::intersect(const Ray &ray, Intersection &intersection)
{
    return root->intersect(ray, intersection);
}

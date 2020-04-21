#include "mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(const std::shared_ptr<Material> &material)
{

}

Mesh::~Mesh()
{
    qDebug() << "Mesh destroyed";
}

void Mesh::setPositions(const QVector<QVector3D>& positions)
{
    this->positions = positions;
}

void Mesh::setVertices(const QVector<Vertex>& vertices)
{
    this->vertices = vertices;
}

void Mesh::setIndices(const QVector<int>& indices)
{
    this->indices = indices;
}

void Mesh::createTriangles()
{
    qDebug() << "indices size" << indices.size();
//    Triangle triangle(positions[0], positions[1], positions[2]);
//    triangles.append(triangle);
    for (int i=0; i<indices.size(); i += 3) {
        Triangle triangle(positions[indices[i]], positions[indices[i+1]], positions[indices[i+2]]);
        triangles.append(triangle);
    }
}

bool Mesh::intersect(const Ray &ray, Intersection &intersection)
{
    bool hit = false;
    for (int i=0; i<triangles.size(); i++) {
        Intersection hitpoint;
        if(triangles[i].intersect(ray, hitpoint)){
            if(hitpoint.distance < intersection.distance){
                intersection = hitpoint;
                hit = true;
//                intersection.objectIndex = i;
            }
        }
    }

//    return intersection.objectIndex != -1;
    return hit;
}

void Mesh::setMaterial(const std::shared_ptr<Material> &material)
{
    this->material = material;
}


Mesh importFbx(const char* filename, const float scale, const QVector3D& offset)
{

    // メモリ管理を行うmanagerを初期化
    FbxManager* manager = FbxManager::Create();

    // IO settingsを作成し、managerに渡す
    FbxIOSettings *ios = FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(ios);

    // managerを使ってimporterを初期化する
    FbxImporter* lImporter = FbxImporter::Create(manager,"");

    // importerをfilenameとio settingで初期化する
    if(!lImporter->Initialize(filename, -1, manager->GetIOSettings())) {
        qDebug() << "Call to FbxImporter::Initialize() failed.";
        qDebug() << "Error returned: %s\n\n" << lImporter->GetStatus().GetErrorString();
        exit(-1);
    }

    // sceneを作成する
    FbxScene* scene = FbxScene::Create(manager,"");

    // sceneにfileをimport
    lImporter->Import(scene);

    // importが終了したらimporterは必要ないので消す
    lImporter->Destroy();

    // polygonを三角形にする
    FbxGeometryConverter geometryConverter(manager);
    geometryConverter.Triangulate(scene, true);

    // オブジェクトを取得する
    FbxNode* node = scene->GetRootNode()->GetChild(0);
    if (node->GetNodeAttribute() == NULL){
        qDebug() << "node attr is NULL";
        exit(-1);
    }

    // オブジェクトからメッシュを取得する
    auto lAttributeType = (node->GetNodeAttribute()->GetAttributeType());
    if(lAttributeType != FbxNodeAttribute::eMesh){
        qDebug() << "attr type is not eMesh";
        exit(-1);
    }
    FbxMesh* fbxmesh = static_cast<FbxMesh*>(node->GetNodeAttribute());

    // positionを取得する
    int numPosition = fbxmesh->GetControlPointsCount();	// 頂点数
    FbxVector4* position = fbxmesh->GetControlPoints();	// position

    // vertexを追加する
    QVector<QVector3D> positions;
    QVector<Vertex> vertices;
    for (int i = 0; i < numPosition; ++i){
        QVector3D pos(position[i][0]*scale, position[i][1]*scale, position[i][2]*scale);
        pos += offset;
        positions.append(pos);
        Vertex v;
        v.position = pos;
        vertices.append(v);
    }

    // indexを取得する
    int numPolygon = fbxmesh->GetPolygonCount();
    QVector<int> indices;
    for (int p = 0; p < numPolygon; ++p){
        //p個目のポリゴンのn個目の頂点への処理
        for (int n = 0; n < 3; ++n){
            // indexを取得
            int index = fbxmesh->GetPolygonVertex(p, n);
            indices.push_back(index);

            // normalを取得
            FbxVector4 normal;
            fbxmesh->GetPolygonVertexNormal(p, n, normal);
            vertices[index].normal = QVector3D(normal[0], normal[1], normal[2]);
        }
    }

    Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setPositions(positions);
    mesh.setIndices(indices);
    mesh.createTriangles();

    // managerと管理しているオブジェクトを全て破棄する
    manager->Destroy();

    return mesh;
}

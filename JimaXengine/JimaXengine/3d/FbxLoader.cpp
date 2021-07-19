#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

const std::string FbxLoader::BASE_DIRECTORY_MODEL = "Resources/model/";
const std::string FbxLoader::BASE_DIRECTORY_TEX = "Resources/texture/";
const std::string FbxLoader::DEFAULT_TEXTURE_FILENAME = "white1x1.png";

void FbxLoader::ParseNodeRecursive(Model* model, FbxNode* fbxNode, Node* parent)
{
    // ノード名を取得
    string neme = fbxNode->GetName();
    // モデルにノードを追加
    model->nodes.emplace_back();
    Node& node = model->nodes.back();
    // ノード名を取得
    node.name = fbxNode->GetName();

    // FBXノードのローカル移動情報
    FbxDouble3 rotation = fbxNode->LclRotation.Get();
    FbxDouble3 scaling = fbxNode->LclScaling.Get();
    FbxDouble3 translation = fbxNode->LclTranslation.Get();

    // 形式変換して代入
    node.rotation = { (float)rotation[0],(float)rotation[1], (float)rotation[2], 0.0f };
    node.scaling = { (float)scaling[0],(float)scaling[1], (float)scaling[2], 0.0f };
    node.translation = { (float)translation[0],(float)translation[1], (float)translation[2], 1.0f };

    // 行列の計算
    XMMATRIX matScaling, matRotation, matTranslation;
    matScaling = XMMatrixScalingFromVector(node.scaling);
    matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
    matTranslation = XMMatrixTranslationFromVector(node.translation);

    // ローカル変形行列の計算
    node.transform = XMMatrixIdentity();
    node.transform *= matScaling;
    node.transform *= matRotation;
    node.transform *= matTranslation;

    // グローバル変換行列の計算
    node.globalTransform = node.transform;
    if (parent)
    {
        node.parent = parent;
        node.globalTransform *= parent->globalTransform;
    }

    // FBXメッシュ情報を解析
    FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

    // fbxNodeAttributeの種類がメッシュだったらメッシュ情報を読み取る
    if (fbxNodeAttribute)
    {
        if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            model->meshNode = &node;
            ParseMesh(model, fbxNode);
        }
    }

    // 子ノードに対して再帰呼び出し
    for (int i = 0; i < fbxNode->GetChildCount(); i++)
    {
        ParseNodeRecursive(model, fbxNode->GetChild(i));
    }
}

void FbxLoader::ParseMesh(Model* model, FbxNode* fbxNode)
{
    // ノードのメッシュを取得
    FbxMesh* fbxMesh = fbxNode->GetMesh();

    // 頂点座標読み取り
    ParseMeshVertices(model, fbxMesh);
    // 面情報読み取り
    ParseMeshFaces(model, fbxMesh);
    // マテリアル読み取り
    ParseMaterial(model, fbxNode);
}

void FbxLoader::ParseMeshVertices(Model* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;

    // 頂点データの数
    const int controlPointCount = fbxMesh->GetControlPointsCount();
    // 頂点データ配列を確保
    Model::FBXVertexData vert{};
    model->vertices.resize(controlPointCount, vert);
    // FBXメッシュの頂点座標配列を取得
    FbxVector4* pCoord = fbxMesh->GetControlPoints();
    // FBXメッシュの全頂点をモデル内の配列にコピーする
    for (int i = 0; i < controlPointCount; i++)
    {
        Model::FBXVertexData& vertex = vertices[i];
        vertex.pos.x = (float)pCoord[i][0];
        vertex.pos.y = (float)pCoord[i][1];
        vertex.pos.z = (float)pCoord[i][2];
    }
}

void FbxLoader::ParseMeshFaces(Model* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;
    auto& indices = model->indices;

    // 複数メッシュ非対応
    assert(indices.size() == 0);
    // 面の数
    const int polygonCount = fbxMesh->GetPolygonCount();
    // UVデータの数
    const int textureUVCount = fbxMesh->GetTextureUVCount();
    // UV名リスト
    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);
    // 面ごとの情報読み取り
    for (int i = 0; i < polygonCount; i++)
    {
        // 面を構成する頂点の数を取得
        const int polygonSize = fbxMesh->GetPolygonSize(i);
        // 三角形じゃなかったら非対応
        assert(polygonSize <= 4);

        // 1頂点ずつ
        for (int j = 0; j < polygonSize; j++)
        {
            // FBX頂点配列のインデックス
            int index = fbxMesh->GetPolygonVertex(i, j);
            // 0以下だったらストップ
            assert(index >= 0);
            // 頂点法線読み込み
            Model::FBXVertexData& vertex = vertices[index];
            FbxVector4 normal;
            if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
            {
                vertex.normal.x = (float)normal[0];
                vertex.normal.y = (float)normal[1];
                vertex.normal.z = (float)normal[2];
            }
            // テクスチャUV読み込み
            if (textureUVCount > 0)
            {
                FbxVector2 uvs;
                bool pUnmappedUV;
                // 0番決め打ち
                if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, pUnmappedUV))
                {
                    vertex.uv.x = (float)uvs[0];
                    vertex.uv.y = (float)uvs[1];
                }
            }
            // 頂点インデックス追加
            // 3頂点目より前なら
            if (j < 3)
            {
                // 頂点を追加して三角形を作る
                indices.push_back(index);
            }
            // 4頂点目なら
            else
            {
                // 3頂点追加して三角形を作る
                int index2 = indices[indices.size() - 1];
                int index3 = index;
                int index0 = indices[indices.size() - 3];
                indices.push_back(index2);
                indices.push_back(index3);
                indices.push_back(index0);
            }
        }
    }
    
}

void FbxLoader::ParseMaterial(Model* model, FbxNode* fbxNode)
{
    const int materialCount = fbxNode->GetMaterialCount();
    // 複数マテリアルに対応してないので0番決め打ち
    if (materialCount > 0)
    {
        // 先頭マテリアルを取得
        FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
        // テクスチャを読み込んだかどうかを表すフラグ
        bool textureLoaded = false;

        if (material)
        {
            // FbxSurfaceLambertクラスかどうかを調べる
            if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

                // 環境光係数
                FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
                model->materialData.ambient.x = (float)ambient.Get()[0];
                model->materialData.ambient.y = (float)ambient.Get()[1];
                model->materialData.ambient.z = (float)ambient.Get()[2];

                // 拡散反射光係数
                FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
                model->materialData.diffuse.x = (float)diffuse.Get()[0];
                model->materialData.diffuse.y = (float)diffuse.Get()[1];
                model->materialData.diffuse.z = (float)diffuse.Get()[2];

            }

            // ディフューズテクスチャを取り出す
            const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (diffuseProperty.IsValid())
            {
                const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
                if (texture)
                {
                    const char* filepath = texture->GetFileName();
                    // ファイルパスからファイル名を抽出
                    string path_str(filepath);
                    string name = ExtractFileName(path_str);
                    // テクスチャ読み込み
                    LoadTexture(model, BASE_DIRECTORY_MODEL + model->name + "/" + name);
                    textureLoaded = true;
                }
            }
        }
        // テクスチャがない場合はデフォルトテクスチャを張る
        if (!textureLoaded)
        {
            LoadTexture(model, BASE_DIRECTORY_TEX + DEFAULT_TEXTURE_FILENAME);
        }
    }
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
    size_t pos1;
    // '\\'が出てくる市場最後の部分を検索
    pos1 = path.rfind('\\');
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }
    // '/'が出てくる一番最後の部分を検索
    pos1 = path.rfind('/');
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }

    return path;
}

void FbxLoader::LoadTexture(Model* model, const std::string& fullpath)
{
    HRESULT result = S_FALSE;
    // WICテクスチャのロード
    TexMetadata& metadata = model->materialData.metadata;
    ScratchImage& scratchImg = model->materialData.scrachImg;
    // ユニコード文字列に変換
    wchar_t wfilepath[128];
    MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath, _countof(wfilepath));
    result = LoadFromWICFile(wfilepath, WIC_FLAGS_NONE, &metadata, scratchImg);
    if (FAILED(result))
    {
        assert(0);
    }
}

FbxLoader& FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return instance;
}

void FbxLoader::Initialize(ID3D12Device* device)
{
    assert(fbxmanager == nullptr);
    this->device = device;
    // マネージャーの生成
    fbxmanager = FbxManager::Create();
    // 入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxmanager, IOSROOT);
    fbxmanager->SetIOSettings(ios);
    // インポーターの生成
    fbxImporter = FbxImporter::Create(fbxmanager, "");
}

void FbxLoader::Finalize()
{
    fbxImporter->Destroy();
    fbxmanager->Destroy();
}

Model* FbxLoader::LoadModelFromFile(const string& modelName)
{
    const string directorypath = BASE_DIRECTORY_MODEL + modelName + "/";
    const string fileName = modelName + ".fbx";
    const string fullpath = directorypath + fileName;

    // ファイルを指定して読み込み
    if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxmanager->GetIOSettings()))
    {
        assert(0);
    }

    // シーン生成
    FbxScene* fbxScene = FbxScene::Create(fbxmanager, "fbxScene");

    // ロードした情報をシーンにインポート
    fbxImporter->Import(fbxScene);

    // モデル生成
    Model* model = new Model();
    model->name = modelName;
    // FBXノード数を取得
    int nodeCount = fbxScene->GetNodeCount();
    // 必要数分メモリを確保
    model->nodes.reserve(nodeCount);
    // ルートノードから順に解析してモデルに入れる
    ParseNodeRecursive(model, fbxScene->GetRootNode());
    // FBXシーン解放
    fbxScene->Destroy();

    // バッファ生成
    model->CreateBuffers(device);

    return model;
}

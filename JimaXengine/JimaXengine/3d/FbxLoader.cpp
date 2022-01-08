#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

const std::string JimaXengine::FbxLoader::BASE_DIRECTORY_MODEL = "Resources/model/";
const std::string JimaXengine::FbxLoader::BASE_DIRECTORY_TEX = "Resources/texture/";
const std::string JimaXengine::FbxLoader::DEFAULT_TEXTURE_FILENAME = "white1x1.png";

JimaXengine::FbxLoader* JimaXengine::FbxLoader::instance = nullptr;

void JimaXengine::FbxLoader::ParseNodeRecursive(Model* model, FbxNode* fbxNode, Node* parent)
{
    //// ノード名を取得
    //string neme = fbxNode->GetName();
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
        // 親の変形を乗算
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
        ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
    }
}

void JimaXengine::FbxLoader::ParseMesh(Model* model, FbxNode* fbxNode)
{
    // ノードのメッシュを取得
    FbxMesh* fbxMesh = fbxNode->GetMesh();

    // 頂点座標読み取り
    ParseMeshVertices(model, fbxMesh);
    // 面情報読み取り
    ParseMeshFaces(model, fbxMesh);
    // マテリアル読み取り
    ParseMaterial(model, fbxNode);
    // スキニング情報の読み取り
    ParseSkin(model, fbxMesh);
}

void JimaXengine::FbxLoader::ParseMeshVertices(Model* model, FbxMesh* fbxMesh)
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

void JimaXengine::FbxLoader::ParseMeshFaces(Model* model, FbxMesh* fbxMesh)
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

    //int cnt = uvNames.GetCount();
    //std::vector<std::string> unNames2;

    //for (int i = 0; i < cnt; i++) {
    //    unNames2.emplace_back(uvNames[i].Buffer());
    //}

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
                    //vertex.uv.y = 1 - (float)uvs[1];    // Mayaの出力に合わせて反転してます
                    vertex.uv.y = -1 * (float)uvs[1];    // Mayaの出力に合わせて反転してます
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

void JimaXengine::FbxLoader::ParseMaterial(Model* model, FbxNode* fbxNode)
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
            if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
            {
                FbxSurfacePhong* phon = static_cast<FbxSurfacePhong*>(material);

                // 環境光係数
                FbxPropertyT<FbxDouble3> ambient = phon->Ambient;
                model->materialData.ambient.x = (float)ambient.Get()[0];
                model->materialData.ambient.y = (float)ambient.Get()[1];
                model->materialData.ambient.z = (float)ambient.Get()[2];

                // 拡散反射光係数
                FbxPropertyT<FbxDouble3> diffuse = phon->Diffuse;
                model->materialData.diffuse.x = (float)diffuse.Get()[0];
                model->materialData.diffuse.y = (float)diffuse.Get()[1];
                model->materialData.diffuse.z = (float)diffuse.Get()[2];

                // 鏡面反射光係数
                FbxPropertyT<FbxDouble3> specular = phon->Specular;
                model->materialData.specular.x = (float)specular.Get()[0];
                model->materialData.specular.y = (float)specular.Get()[1];
                model->materialData.specular.z = (float)specular.Get()[2];
            }
            // FbxSurfaceLambertクラスかどうかを調べる
            else if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                FbxSurfacePhong* phon = static_cast<FbxSurfacePhong*>(material);

                // 環境光係数
                FbxPropertyT<FbxDouble3> ambient = phon->Ambient;
                model->materialData.ambient.x = (float)ambient.Get()[0];
                model->materialData.ambient.y = (float)ambient.Get()[1];
                model->materialData.ambient.z = (float)ambient.Get()[2];

                // 拡散反射光係数
                FbxPropertyT<FbxDouble3> diffuse = phon->Diffuse;
                model->materialData.diffuse.x = (float)diffuse.Get()[0];
                model->materialData.diffuse.y = (float)diffuse.Get()[1];
                model->materialData.diffuse.z = (float)diffuse.Get()[2];

                // 鏡面反射光係数
                //FbxPropertyT<FbxDouble3> specular = phon->Specular;
                model->materialData.specular.x = 0.5f;
                model->materialData.specular.y = 0.5f;
                model->materialData.specular.z = 0.5f;
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

std::string JimaXengine::FbxLoader::ExtractFileName(const std::string& path)
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

void JimaXengine::FbxLoader::LoadTexture(Model* model, const std::string& fullpath)
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

void JimaXengine::FbxLoader::ParseSkin(Model* model, FbxMesh* fbxMesh)
{
    //////////////////////////////
    // スキニング情報読み取り
    ////////////////////////////// 
    
    // スキニング情報
    FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
    // スキニング情報がなければ
    if (fbxSkin == nullptr)
    {
        // 各項目について
        for (int i = 0; i < model->vertices.size(); i++)
        {
            model->vertices[i].boneIndex[0] = 0;
            model->vertices[i].boneWeight[0] = 1.0f;
        }
        return;
    }

    // ボーンの配列の参照
    std::vector<Model::Bone>& bones = model->bones;

    // ボーンの数
    int clusterCount = fbxSkin->GetClusterCount();
    bones.reserve(clusterCount);

    // 全ボーンで
    for (int i = 0; i < clusterCount; i++)
    {
        // FBXボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
        // ボーンんの名前を取得
        const char* boneName = fbxCluster->GetLink()->GetName();

        // 新しくボーンを追加して、参照を得る
        bones.emplace_back(Model::Bone(boneName));
        Model::Bone& bone = bones.back();
        // 作ってあるボーンとFBXのボーンを紐づける
        bone.fbxCluster = fbxCluster;
        // FBXから初期姿勢行列を取得する
        FbxAMatrix fbxMat;
        fbxCluster->GetTransformLinkMatrix(fbxMat);

        // XMMATRIX型に変換する
        XMMATRIX initialPose;
        ConvertMatrixFromFbx(&initialPose, fbxMat);

        // 初期姿勢行列の逆行列を得る
        bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
    }

    //////////////////////////////
    // スキンウェイトの読み取り
    ////////////////////////////// 
    
    // ボーン番号とスキンウェイトのペア
    struct WeightSet
    {
        UINT index;
        float weight;
    };

    // 2次元行列（ジャグ配列）
    // list:頂点が影響を受けるボーンの全リスト
    // vector:それを全頂点分
    std::vector<std::list<WeightSet>> weightLists(model->vertices.size());

    // 全ボーンで
    for (int i = 0; i < clusterCount; i++)
    {
        // FBXボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
        // このボーンに影響を受ける頂点の数
        int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
        // このボーンに影響を受ける頂点配列
        int* controlPointIndices = fbxCluster->GetControlPointIndices();
        double* controlPointWeights = fbxCluster->GetControlPointWeights();

        // 影響を受ける頂点で
        for (int j = 0; j < controlPointIndicesCount; j++)
        {
            // 頂点番号
            int vertIndex = controlPointIndices[j];
            // スキンウェイト
            float weight = (float)controlPointWeights[j];
            // その頂点に影響を与えるボーンリストに、ボーンとウェイトのペアを追加
            weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i, weight });
        }
    }
    
    //////////////////////////////
    // スキンウェイトの整理
    ////////////////////////////// 

    // 頂点配列書き換え用の参照
    auto& vertices = model->vertices;
    // 各頂点について
    for (int i = 0; i < vertices.size(); i++)
    {
        // 頂点のウェイトから最も大きい4つを選択
        auto& weightList = weightLists[i];
        // 大小比較用のラムダ式を指定して降順にソート
        weightList.sort([](auto const& lhs, auto const& rhs)
        {
            return lhs.weight > rhs.weight;
        });
        int ewightArrayIndex = 0;
        // 降順ソート済みにウェイトリストから
        for (auto& weightSet : weightList)
        {
            // 頂点データ書き込み
            vertices[i].boneIndex[ewightArrayIndex] = weightSet.index;
            vertices[i].boneWeight[ewightArrayIndex] = weightSet.weight;
            // 4つになったら終了
            if (++ewightArrayIndex >= Model::MAX_BONE_INDOCES)
            {
                float weight = 0.0f;
                // 2番目以降のウェイトを合計
                for (int j = 1; j < Model::MAX_BONE_INDOCES; j++)
                {
                    weight += vertices[i].boneWeight[j];
                }
                // 合計で1.0fになるように調整
                vertices[i].boneWeight[0] = 1.0f - weight;
                break;
            }
        }
    }
}

void JimaXengine::FbxLoader::ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, FbxAMatrix& src)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dst->r[i].m128_f32[j] = (float)src.Get(i, j);
        }
    }
}

JimaXengine::FbxLoader& JimaXengine::FbxLoader::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new FbxLoader();
    }
    return *instance;
}

void JimaXengine::FbxLoader::Initialize(ID3D12Device* device)
{
    // 初期化チェック
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

void JimaXengine::FbxLoader::Finalize()
{
    // FBXインスタンスの破棄
    fbxImporter->Destroy();
    fbxmanager->Destroy();
}

void JimaXengine::FbxLoader::LoadModelFromFiletoBuff(const string& modelName)
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

    // FBXシーン設定
    model->fbxScene = fbxScene;

    // バッファ生成
    model->CreateBuffers(device);

    models.emplace(modelName, model);
}

JimaXengine::Model* JimaXengine::FbxLoader::GetFbxModel(const std::string& modelName)
{
    return models[modelName];
}

#include "FbxLoader.h"
#include <cassert>

const std::string FbxLoader::BASE_DIRECTORY = "Resources/";

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

void FbxLoader::LoadModelFromFile(const string& modelName)
{
    const string directorypath = BASE_DIRECTORY + modelName + "/";
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

}

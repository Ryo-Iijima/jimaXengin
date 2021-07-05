// fbxモデルを読み込むクラス

#pragma once
#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>

#include <string>

#include "Model.h"

class FbxLoader
{
private:	// エイリアス
	using string = std::string;

public:		// 定数
	// ファイルパスの前半の共通部分
	static const string BASE_DIRECTORY;
	// マテリアル用デフォルトテクスチャ
	static const string DEFAULT_TEXTURE_FILENAME;
private:	// 変数
	
	ID3D12Device* device = nullptr;
	FbxManager* fbxmanager = nullptr;
	FbxImporter* fbxImporter = nullptr;

private:	// シングルトンの決めごと
	// コンストラクタをprivateにして
	// コピーと代入を禁止する
	FbxLoader() = default;
	~FbxLoader() = default;
	FbxLoader(const FbxLoader& obj) = delete;
	void operator=(const FbxLoader& obj) = delete;

private:	// 関数

	/// <summary>
	/// ノード構成を再帰的に解析
	/// </summary>
	/// <param name="model">読み込み先のモデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(Model* model, FbxNode* fbxNode, Node* parent=nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="model">読み込み先のモデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseMesh(Model* model, FbxNode* fbxNode);

	// メッシュ読み取り用のサブ関数
	// 頂点読み取り
	void ParseMeshVertices(Model* model, FbxMesh* fbxMesh);
	// 面情報読み取り
	void ParseMeshFaces(Model* model, FbxMesh* fbxMesh);
	// マテリアル読み取り
	void ParseMaterial(Model* model, FbxNode* fbxNode);
	// ディレクトリを含んだファイルパスからファイル名だけを抽出する
	std::string ExtractFileName(const std::string& path);
	// テクスチャ読み取り
	void LoadTexture(Model* model, const std::string& fullpath);


public:		// 関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader& GetInstance();

	void Initialize(ID3D12Device* device);

	void Finalize();

	/// <summary>
	/// モデルファイル読み込み
	/// </summary>
	/// <param name="modelName">ファイル名</param>
	/// <returns>モデルデータ</returns>
	Model* LoadModelFromFile(const string& modelName);


};
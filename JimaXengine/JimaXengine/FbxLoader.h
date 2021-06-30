#pragma once
#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>

#include <string>

class FbxLoader
{
private:	// エイリアス
	using string = std::string;

public:		// 定数
	static const string BASE_DIRECTORY;

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
	void LoadModelFromFile(const string& modelName);

};
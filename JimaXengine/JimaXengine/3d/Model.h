/// 読み込んだモデル情報を入れるクラス

#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <fbxsdk.h>

/// <summary>
/// ノード１つ分の情報構造体
/// </summary>
struct Node
{
	// 名前
	std::string name;
	// ローカルスケール
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	// ローカル回転角
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	// ローカル移動
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	// ローカル変形行列
	DirectX::XMMATRIX transform;
	// グローバル変形行列
	DirectX::XMMATRIX globalTransform;
	// 親ノード
	Node* parent = nullptr;
};

class Model
{
private:	// エイリアス
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template <class T>using vector = std::vector<T>;

	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// フレンドクラス

	friend class FbxLoader;

public:		// 定数
	// ボーンインデックスの最大数
	static const int MAX_BONE_INDOCES = 4;

public:		// サブクラス

	// FBX頂点データ構造体
	struct FBXVertexData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDOCES];	// ボーン番号
		float boneWeight[MAX_BONE_INDOCES]; // ボーン重み
	};

	// FBXマテリアルデータ構造体
	struct FBXMaterialData
	{
		// アンビエント係数
		DirectX::XMFLOAT3 ambient = { 1,1,1 };
		// ディフューズ係数
		DirectX::XMFLOAT3 diffuse = { 1,1,1 };
		// テクスチャメタデータ
		DirectX::TexMetadata metadata = {};
		// スクラッチイメージ
		DirectX::ScratchImage scrachImg = {};
	};

	// ボーン構造体
	struct Bone
	{
		std::string name;
		DirectX::XMMATRIX invInitialPose;	// 初期姿勢の逆行列		
		FbxCluster* fbxCluster;				// クラスター（FBX側のボーン情報）
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};


private:	//変数
	// モデル名
	std::string name;
	// ノード配列
	std::vector<Node> nodes;
	// メッシュを持つノード
	Node* meshNode = nullptr;
	// 頂点データ配列
	std::vector<FBXVertexData> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;
	// マテリアルデータ
	FBXMaterialData materialData;
	// ボーン配置
	std::vector<Bone> bones;
	// FBXシーン
	FbxScene* fbxScene = nullptr;

	// 各種バッファ・ビュー
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	
public:		// 関数
	~Model();

	// バッファ生成
	void CreateBuffers(ID3D12Device* device);

	void Draw(ID3D12GraphicsCommandList* cmdList);

	// getter
	// モデルの変形行列取得
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
	std::vector<Bone>& GetBonse() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
};
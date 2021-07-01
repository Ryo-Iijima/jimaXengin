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
#include <wrl.h>

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

public:		// サブクラス

	// FBX頂点データ構造体
	struct FBXVertexData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
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
	// バッファ生成
	void CreateBuffers(ID3D12Device* device);
};
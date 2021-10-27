#pragma once

static const int MAX_BONE_INDOCES = 4;

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

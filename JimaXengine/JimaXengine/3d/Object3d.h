#pragma once

#include "Model.h"
#include "../Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include "../DirectXCommon.h"
#include <map>
#include "../math/Vector4.h";

class Input;
class Object3d
{
private:

	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template <class T>using vector = std::vector<T>;
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	static WinApp* winApp;
	static DirectXCommon* dxCommon;
	static ComPtr<ID3D12RootSignature> rootsignature;
	static std::map<std::string, ComPtr<ID3D12PipelineState>> pipelines;
	static ComPtr<ID3D12DescriptorHeap> basicDescHeap;
	static const int SRV_MAX_COUNT = 256;
	static UINT texNumber;
	static std::map<std::string, UINT> textureMap;

	Camera* camera = nullptr;
	Object3d* parent = nullptr;

	//ComPtr<ID3D12Resource> constBuff;
	ComPtr<ID3D12Resource> constBufferTranceform;
	ComPtr<ID3D12Resource> constBufferSkin;

	Vector3 scale = { 1,1,1 };
	Vector3 rotation = { 0,0,0 };
	Vector3 position = { 0,0,0 };
	XMMATRIX matWorld;
	XMMATRIX matWorldViewProj;
	XMMATRIX matScale;
	XMMATRIX matRotation;
	XMMATRIX matTrans;

	Vector3 localY;

	// アニメーション再生関連
	// 1フレームの時間
	FbxTime frameTime;
	// アニメーション開始時間
	FbxTime startTime;
	// アニメーション終了時間
	FbxTime endTime;
	// 現在時間
	FbxTime currentTime;
	// アニメーション再生中
	bool isPlay = false;

private:

	static HRESULT CreateDescriptorHeap();
	static HRESULT CreateRootSignature();
	static HRESULT CreateGraphicsPipeline(const std::string& vsfilename, const std::string& psfilename, const std::string& registername);

	HRESULT CreateConstBuffer();
	void UpdateMatrix(Vector3 pos, Vector3 rot, Vector3 scale, DirectX::XMMATRIX billboardMat, Vector3 localRot);
	HRESULT TransferConstBuffer(Camera* camera, Vector4 color, Vector3 light);
	void DrawCommands(const std::string& modelname, const std::string& filename, const std::string& shadername);

	void PlayAnimation();

public:

	Object3d();
	~Object3d();
	
	static void Initialize(WinApp* winapp,DirectXCommon* dxcommon);
	static void LoadResourceTexture(const std::string& filename);
	void CreateMesh();
	void SetParent(Object3d* parent);
	void Update(Vector3 pos, Vector3 rot, Vector3 scale, DirectX::XMMATRIX billboardMat = DirectX::XMMatrixIdentity(), Vector3 localRot = Vector3::Zero);
	void Draw(Camera* camera, 
		const std::string& modelname,
		const std::string& filename,
		const std::string& registername,
		const std::string& blendtype,
		const std::string& pipelinecustom = "DEFAULT",
		Vector4 color = { 1,1,1,1 },
		Vector3 light = { 0,500,0 });

	Vector3 GetLocalX();
	Vector3 GetLocalY();
	Vector3 GetLocalZ();
	Vector3 GetRotation() { return rotation; }
	Vector3 GetPosition() { return position; }
	Vector3 GetScale() { return scale; }

	// カメラ操作（実験）
	void CameraMove();

	// setter
	void SetModel(Model* model) { this->model = model; }
	void SetInput(Input* input) { this->input = input; }

/// ここから上に切り離す ////////////////////////////////////////////////////////////////////////////


private:		// 定数
	// ボーンの最大数
	static const int MAX_BONES = 32;

	enum ViewName
	{
		transform,
		texture,
		skin,
	};

	struct ConstBfferData
	{
		XMMATRIX matrix;
		Vector4 color;
	};

public:		// サブクラス
	// 座標変換行列(CB)用構造体
	struct TransformData
	{
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};
	// スキニング情報(CB)用構造体
	struct SkinData
	{
		XMMATRIX bones[MAX_BONES];
	};
protected:


private:	// 変数
	Model* model = nullptr;

	//static ID3D12Device* _dev;


	Vector3 eye;
	Vector3 target;
	Vector3 up;

	Input* input;


private:	// 関数

	//static void SetUpRootParameter();
	//static void SetUpSampler();
	//static void SetDevice(ID3D12Device* device) { Object3d::_dev = device; }


};


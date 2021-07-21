#pragma once

#include "Model.h"
#include "../Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

class Input;
class Object3d
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

public:		// サブクラス
	// 座標変換行列(CB)用構造体
	struct TransformData
	{
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};
protected:

	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// ローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	// ローカルワールド変換行列
	XMMATRIX matWorld;
	// モデル
	Model* model = nullptr;
	// カメラ
	Camera* camera = nullptr;

private:	// static変数

	static ID3D12Device* _dev;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature;
	// パイプラインステート
	static ComPtr<ID3D12PipelineState> piplineState;


public:		// static関数

	static void CreateGraphicsPipline();

	/// <summary>
	/// ルートパラメーターの設定
	/// </summary>
	static void SetUpRootParameter();

	/// <summary>
	/// サンプラーの設定
	/// </summary>
	/// <returns>成否</returns>
	static void SetUpSampler();

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	/// <returns>成否</returns>
	static bool CreatRootSignature();

	// setter
	static void SetDevice(ID3D12Device* device) { Object3d::_dev = device; }

private:	// 変数

	ComPtr<ID3D12Resource> constBufferTranceform;

	Vector3 eye;
	Vector3 target;
	Vector3 up;

	Input* input;


public:		// 関数

	void Initialize();

	void Update();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	// カメラ操作（実験）
	void CameraMove();

	// setter
	void SetModel(Model* model) { this->model = model; }

	void SetInput(Input* input) { this->input = input; }
};


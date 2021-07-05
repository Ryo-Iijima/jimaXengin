#pragma once

#include "Model.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

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

private:	// static変数

	static ID3D12Device* device;

public:		// static関数

	// setter
	static void SetDevice(ID3D12Device* device) { Object3d::device = device; }

private:	// 変数

	ComPtr<ID3D12Resource> constBufferTranceform;

public:		// 関数

	void Initialize();

};


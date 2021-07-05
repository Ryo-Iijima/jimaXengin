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
private:	// �G�C���A�X
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template <class T>using vector = std::vector<T>;

	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// �T�u�N���X
	// ���W�ϊ��s��(CB)�p�\����
	struct TransformData
	{
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};

private:	// static�ϐ�

	static ID3D12Device* device;

public:		// static�֐�

	// setter
	static void SetDevice(ID3D12Device* device) { Object3d::device = device; }

private:	// �ϐ�

	ComPtr<ID3D12Resource> constBufferTranceform;

public:		// �֐�

	void Initialize();

};


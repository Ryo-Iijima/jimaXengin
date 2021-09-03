#pragma once

#include "Model.h"
#include "../Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include "../math/Vector4.h"
#include "../general/Input.h"

class DirectXCommon;
class WinApp;
//class Input;
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

private:		// �萔
	// �{�[���̍ő吔
	static const int MAX_BONES = 256;

	enum ViewName
	{
		transform,
		texture,
		skin,
	};
public:		// �T�u�N���X
	// ���W�ϊ��s��(CB)�p�\����
	struct TransformData
	{
		Vector4 color;
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};
	// �X�L�j���O���(CB)�p�\����
	struct SkinData
	{
		XMMATRIX bones[MAX_BONES];
	};
protected:

	// �F
	Vector4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// ���[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;
	// ���f��
	Model* model = nullptr;
	// �J����
	Camera* camera = nullptr;

private:	// static�ϐ�

	static DirectXCommon* dxCommon;

	static ID3D12Device* _dev;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature;
	// �p�C�v���C���X�e�[�g
	static ComPtr<ID3D12PipelineState> piplineState;


public:		// static�֐�
	static void StaticInitialize(DirectXCommon* dxcommon, WinApp* winapp);

	static void CreateGraphicsPipline();

	/// <summary>
	/// ���[�g�p�����[�^�[�̐ݒ�
	/// </summary>
	static void SetUpRootParameter();

	/// <summary>
	/// �T���v���[�̐ݒ�
	/// </summary>
	/// <returns>����</returns>
	static void SetUpSampler();

	/// <summary>
	/// ���[�g�V�O�l�`���̍쐬
	/// </summary>
	/// <returns>����</returns>
	static bool CreatRootSignature();

	// setter
	static void SetDevice(ID3D12Device* device) { Object3d::_dev = device; }

private:	// �ϐ�

	ComPtr<ID3D12Resource> constBufferTranceform;
	ComPtr<ID3D12Resource> constBufferSkin;

	Vector3 eye;
	Vector3 target;
	Vector3 up;

	Input* input;

	// �A�j���[�V�����Đ��֘A
	// 1�t���[���̎���
	FbxTime frameTime;
	// �A�j���[�V�����J�n����
	FbxTime startTime;
	// �A�j���[�V�����I������
	FbxTime endTime;
	// ���ݎ���
	FbxTime currentTime;
	// �A�j���[�V�����Đ���
	bool isPlay = false;

private:	// �֐�
	/// <summary>
	/// �A�j���[�V�����Đ�
	/// </summary>
	void PlayAnimation();

public:		// �֐�

	void Initialize();

	void Update();

	//void Draw(ID3D12GraphicsCommandList* cmdList);
	void Draw();

	// �J��������i�����j
	void CameraMove();

	// setter
	void SetModel(Model* model) { this->model = model; }

	void SetColor(Vector4& color) { this->color = color; }
	void SetScale(Vector3& scale) { this->scale = scale; }
	void SetRotation(Vector3& rotation) { this->rotation = rotation; }
	void SetPosition(Vector3& position) { this->position = position; }

	void SetCamera(Camera* camera) { this->camera = camera; }
	void SetInput(Input* input) { this->input = input; }
};


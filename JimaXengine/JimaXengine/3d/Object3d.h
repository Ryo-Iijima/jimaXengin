#pragma once

#include "Model.h"
#include "../camera/Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include "../math/Vector4.h"
#include "../general/Input.h"
#include "../DirectXCommon.h"
#include "../LightGroup.h"

namespace JimaXengine
{
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

		enum class ViewName
		{
			transform,
			texture,
			skin,
			light,
			material,
		};
	public:		// �T�u�N���X
		// ���W�ϊ��s��(CB0)�p�\����
		struct TransformData
		{
			Vector4 color;
			XMMATRIX viewproj;
			XMMATRIX world;
			XMFLOAT3 cameraPos;
		};
		// �X�L�j���O���(CB3)�p�\����
		struct SkinData
		{
			XMMATRIX bones[MAX_BONES];
		};
	protected:

		// �F
		Vector4& color;
		// ���[�J���X�P�[��
		XMFLOAT3& scale;
		// ���[�J����]�p
		XMFLOAT3& rotation;
		// ���[�J�����W
		XMFLOAT3& position;

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
		static ComPtr<ID3D12PipelineState> pipelineState;

		//���C�g
		static LightGroup* lightGroup;

		// �r���{�[�h���ǂ���
		bool isBillboard = false;

	public:		// static�֐�
		static void StaticInitialize(DirectXCommon* dxcommon, WinApp* winapp);

		static void CreateGraphicsPipeline();

		// setter
		static void SetDevice(ID3D12Device* device) { Object3d::_dev = device; }
		static void SetLightGroup(LightGroup* plight) { lightGroup = plight; }

	private:	// �ϐ�

		ComPtr<ID3D12Resource> constBufferTransform;
		ComPtr<ID3D12Resource> constBufferSkin;

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
		// �w�肵���I�����Ԃ܂ōĐ�����
		bool isEndAnimation = false;
		// �A�j���[�V���������[�v����
		bool isAnimationLoop = true;
		

	private:	// �֐�
		// �A�j���[�V�����֘A������
		void AnimationInit();
		// �A�j���[�V�����Đ�
		bool PlayAnimation(bool loop);

	public:		// �֐�

		Object3d();
		Object3d(Vector3& pos, Vector3& scale, Vector3& rot, Vector4& color);

		void Initialize();

		void Update();

		void Draw();

		// setter
		void SetModelforBuff(const std::string& modelName);
		void SetCamera(Camera* camera) { this->camera = camera; }
		void SetIsBillboard(bool isbillbord) { isBillboard = isbillbord; }

		void SetAnimationLoop(bool loop) { isAnimationLoop = loop; }
		void SetAnimationFrame(const int start, const int end, const int frametime = 1);

		// getter
		bool GetIsEndAnimation() { return isEndAnimation; }
		LightGroup* GetUsingLightGroup() { return lightGroup; }
	};
}
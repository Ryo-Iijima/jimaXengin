#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"

namespace JimaXengine
{
	/// <summary>
	/// ���C�g�O���[�v
	/// </summary>
	class LightGroup
	{
	private: // �G�C���A�X
		// Microsoft::WRL::���ȗ�
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::���ȗ�
		using XMVECTOR = DirectX::XMVECTOR;

	public: // �萔
		// ���s�����̐�
		static const int DirLightNum = 3;
		// �|�C���g���C�g�̐�
		static const int PointLightNum = 3;
		// �X�|�b�g���C�g�̐�
		static const int SpotLightNum = 3;
		// �ۉe�̐�
		static const int CircleShadowNum = 1;

	public: // �T�u�N���X

		// �萔�o�b�t�@�p�f�[�^�\����
		struct ConstBufferData
		{
			// �����̐F
			Vector3 ambientColor;
			float pad1;
			// ���s�����p
			DirectionalLight::ConstBufferData dirLights[DirLightNum];
			// �_�����p
			PointLight::ConstBufferData pointLights[PointLightNum];
			// �X�|�b�g���C�g�p
			SpotLight::ConstBufferData spotLights[SpotLightNum];
			// �ۉe�p
			CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
		};

	public: // �ÓI�����o�֐�
		/// <summary>
		/// �ÓI������
		/// </summary>
		/// <param name="device">�f�o�C�X</param>
		static void StaticInitialize(ID3D12Device* device);

		/// <summary>
		/// �C���X�^���X����
		/// </summary>
		/// <returns>�C���X�^���X</returns>
		static LightGroup* Create();

	private: // �ÓI�����o�ϐ�
		// �f�o�C�X
		static ID3D12Device* device;

	public: // �����o�֐�
		/// <summary>
		/// ������
		/// </summary>
		void Initialize();

		/// <summary>
		/// �X�V
		/// </summary>
		void Update();

		/// <summary>
		/// �`��
		/// </summary>
		void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

		/// <summary>
		/// �萔�o�b�t�@�]��
		/// </summary>
		void TransferConstBuffer();

		/// <summary>
		/// �W���̃��C�g�ݒ�
		/// </summary>
		void DefaultLightSetting();

		// setter

		/// <summary>
		/// �����̃��C�g�F���Z�b�g
		/// </summary>
		/// <param name="color">���C�g�F</param>
		void SetAmbientColor(const Vector3& color);

		/////////////////////////
		/// ���s����
		/////////////////////////

		/// <summary>
		/// ���s�����̗L���t���O���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="active">�L���t���O</param>
		void SetDirLightActive(int index, bool active);

		/// <summary>
		/// ���s�����̃��C�g�������Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightdir">���C�g����</param>
		void SetDirLightDir(int index, const XMVECTOR& lightdir);

		/// <summary>
		/// ���s�����̃��C�g�F���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightcolor">���C�g�F</param>
		void SetDirLightColor(int index, const Vector3& lightcolor);

		/////////////////////////
		/// �|�C���g���C�g
		/////////////////////////

		/// <summary>
		/// �_�����̗L���t���O���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="active">�L���t���O</param>
		void SetPointLightActive(int index, bool active);

		/// <summary>
		/// �_�����̃��C�g�ʒu���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightdir">���C�g�ʒu</param>
		void SetPointLightPos(int index, const Vector3& lightpos);

		/// <summary>
		/// �_�����̃��C�g�F���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightcolor">���C�g�F</param>
		void SetPointLightColor(int index, const Vector3& lightcolor);

		/// <summary>
		/// �_�����̌��������Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightAtten">������</param>
		void SetPointLightAtten(int index, const Vector3& lightAtten);

		/////////////////////////
		/// �X�|�b�g���C�g
		/////////////////////////

		/// <summary>
		/// �X�|�b�g���C�g�̗L���t���O���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="active">�L���t���O</param>
		void SetSpotLightActive(int index, bool active);

		/// <summary>
		/// �X�|�b�g���C�g�̃��C�g�������Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightdir">���C�g����</param>
		void SetSpotDir(int index, const XMVECTOR& lightdir);

		/// <summary>
		/// �X�|�b�g���C�g�̈ʒu���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightdir">���C�g�ʒu</param>
		void SetSpotLightPos(int index, const Vector3& lightpos);

		/// <summary>
		/// �X�|�b�g���C�g�̐F���Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightcolor">���C�g�F</param>
		void SetSpotLightColor(int index, const Vector3& lightcolor);

		/// <summary>
		/// �X�|�b�g���C�g�̌��������Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightAtten">������</param>
		void SetSpotLightAtten(int index, const Vector3& lightAtten);

		/// <summary>
		/// �X�|�b�g���C�g�̌����p�x�̃Z�b�g
		/// </summary>
		/// <param name="index">���C�g�ԍ�</param>
		/// <param name="lightFactorAngle">�����p�x</param>
		void SetSpotLightFactorAngle(int index, const Vector2& lightFactorAngle);

		/////////////////////////
		/// �ۉe
		/////////////////////////

		/// <summary>
		/// �ۉe�̗L���t���O
		/// </summary>
		/// <param name="index">�ۉe�ԍ�</param>
		/// <param name="activ">�L���t���O</param>
		void SetCircleShadowActiv(int index, bool activ);

		/// <summary>
		/// �L���X�^�[�̈ʒu�̃Z�b�g
		/// </summary>
		/// <param name="index">�ۉe�̔ԍ�</param>
		/// <param name="casterpos">�L���X�^�[�̈ʒu</param>
		void SetCircleShadowCasterPos(int index, const Vector3& casterpos);

		/// <summary>
		/// �ۉe�̓��e�����̃Z�b�g
		/// </summary>
		/// <param name="index">�ۉe�̔ԍ�</param>
		/// <param name="dir">���e����</param>
		void SetCircleShadowDir(int index, const XMVECTOR& dir);

		/// <summary>
		/// �L���X�^�[�ƃ��C�g�̋����̃Z�b�g
		/// </summary>
		/// <param name="index">�ۉe�̔ԍ�</param>
		/// <param name="distancecasterlight">�L���X�^�[�ƃ��C�g�̋���</param>
		void SetCircleShadowDistanceCasterLight(int index, float distancecasterlight);

		/// <summary>
		/// �ۉe�̌������̃Z�b�g
		/// </summary>
		/// <param name="index">�ۉe�̔ԍ�</param>
		/// <param name="atten">������</param>
		void SetCircleShadowAtten(int index, const Vector3& atten);

		/// <summary>
		/// �ۉe�̌����p�x
		/// </summary>
		/// <param name="index">�ۉe�̔ԍ�</param>
		/// <param name="factorangle">�����p�x</param>
		void SetCircleShadowFactorAngle(int index, const Vector2& factorangle);

	private: // �����o�ϐ�
		// �萔�o�b�t�@
		ComPtr<ID3D12Resource> constBuff;

		// �����̐F
		Vector3 ambientColor = { 1,1,1 };

		// ���s�����̔z��
		DirectionalLight dirLights[DirLightNum];

		// �|�C���g���C�g�̔z��
		PointLight pointLights[PointLightNum];

		// �X�|�b�g���C�g�̔z��
		SpotLight spotLights[SpotLightNum];

		// �ۉe�̔z��
		CircleShadow circleShadows[CircleShadowNum];

		// �_�[�e�B�t���O
		bool dirty = false;
	};

}
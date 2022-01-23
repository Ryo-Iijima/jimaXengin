#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "DirectionalLight.h"
#include "PointLight.h"

namespace JimaXengine
{
	/// <summary>
	/// ���C�g
	/// </summary>
	class LightGroup
	{
	private: // �G�C���A�X
		// Microsoft::WRL::���ȗ�
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::���ȗ�
		using XMVECTOR = DirectX::XMVECTOR;

	public: // �萔
		static const int DirLightNum = 3;
		static const int PointLightNum = 3;

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

		// ���s����

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

		// �_����

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
		/// <param name="index"></param>
		/// <param name="lightAtten"></param>
		void SetPointLightAtten(int index, const Vector3& lightAtten);

	private: // �����o�ϐ�
		// �萔�o�b�t�@
		ComPtr<ID3D12Resource> constBuff;

		// �����̐F
		Vector3 ambientColor = { 1,1,1 };

		// ���s�����̔z��
		DirectionalLight dirLights[DirLightNum];

		// �_�����̔z��
		PointLight pointLights[PointLightNum];

		// �_�[�e�B�t���O
		bool dirty = false;
	};

}
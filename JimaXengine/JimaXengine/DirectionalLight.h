#pragma once

#include <DirectXMath.h>
#include "math/Vector3.h"

namespace JimaXengine
{
	/// <summary>
	/// ���s����
	/// </summary>
	class DirectionalLight
	{
	private: // �G�C���A�X
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;

	public: // �T�u�N���X

		// �萔�o�b�t�@�p�f�[�^�\����
		struct ConstBufferData
		{
			XMVECTOR lightv;
			Vector3 lightcolor;
			unsigned int active;
		};

	public: // �����o�֐�
		// setter
		inline void SetLightDir(const XMVECTOR& lightdir) { this->lightdir = DirectX::XMVector3Normalize(lightdir); }
		inline void SetLightColor(const Vector3& lightcolor) { this->lightcolor = lightcolor; }
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const XMVECTOR& GetLightDir() { return lightdir; }
		inline const Vector3& GetLightColor() { return lightcolor; }
		inline bool IsActive() { return active; }

	private: // �����o�ϐ�
		// ���C�g�����i�P�ʃx�N�g���j
		XMVECTOR lightdir = { 1,0,0,0 };
		// ���C�g�F
		Vector3 lightcolor = { 1,1,1 };
		// �L���t���O
		bool active = false;
	};

}
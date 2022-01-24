#pragma once
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "general/General.h"

namespace JimaXengine
{
	/// <summary>
	/// �X�|�b�g���C�g
	/// </summary>
	class SpotLight
	{
	private: // �G�C���A�X
		using XMVECTOR = DirectX::XMVECTOR;

	public: // �T�u�N���X
		// �萔�o�b�t�@�p�\����
		struct ConstBufferData
		{
			XMVECTOR lightv;
			Vector3 lightPos;
			float pad1;
			Vector3 lightColor;
			float pad2;
			Vector3 lightAtten;
			float pad3;
			Vector2 lightFactorAngleCos;
			unsigned int active;
			float pad4;
		};

	private: // �����o�ϐ�
		// ���C�g�����i�P�ʃx�N�g���j
		XMVECTOR lightdir = { 1,0,0,0 };
		// ���W
		Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
		// �F
		Vector3 lightColor = { 1.0f, 1.0f, 1.0f };
		// ���������W��
		Vector3 lightAtten = { 1.0f, 1.0f, 1.0f };
		// ���C�g�����p�x
		Vector2 lightFactorAngleCos = { 0.5f,0.2f };
		// �L���t���O
		bool active = false;

	public: // �����o�֐�
		// setter
		inline void SetLightDir(const XMVECTOR& lightdir) { this->lightdir = DirectX::XMVector3Normalize(lightdir); }
		inline void SetLightPos(const Vector3& lightpos) { lightPos = lightpos; }
		inline void SetLightColor(const Vector3& lightcolor) { lightColor = lightcolor; }
		inline void SetLightAtten(const Vector3& lightatten) { lightAtten = lightatten; }
		inline void SetLightFactorAngle(const Vector2& lightfactorangle) { 
			lightFactorAngleCos.x = cosf(General::ConvertToRadians(lightfactorangle.x));
			lightFactorAngleCos.y = cosf(General::ConvertToRadians(lightfactorangle.y));
		}
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const XMVECTOR& GetLightDir() { return lightdir; }
		inline const Vector3& GetLightPos() { return lightPos; }
		inline const Vector3& GetLightColor() { return lightColor; }
		inline const Vector3& GetLightAtten() { return lightAtten; }
		inline const Vector2& GetLightFactorAngleCos() { return lightFactorAngleCos; }
		inline const bool IsActive() { return active; }
	};
}
#pragma once
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "general/General.h"

namespace JimaXengine
{
	/// <summary>
	/// �ۉe
	/// </summary>
	class CircleShadow
	{
	private: // �G�C���A�X
		using XMVECTOR = DirectX::XMVECTOR;

	public: // �T�u�N���X
		// �萔�o�b�t�@�p�\����
		struct ConstBufferData
		{
			XMVECTOR dir;
			Vector3 casterPos;
			float distanceCasterPos;
			Vector3 atten;
			float pad1;
			Vector2 factorAngleCos;
			unsigned int active;
			float pad2;
		};

	private: // �����o�ϐ�
		// �����i�P�ʃx�N�g���j
		XMVECTOR dir = { 1,0,0,0 };
		// �L���X�^�[�ƃ��C�g�̋���
		float distanceCasterLight = 100.0f;
		// �L���X�^�[���W
		Vector3 casterPos = { 0.0f, 0.0f, 0.0f };
		// ���������W��
		Vector3 atten = { 0.5f, 0.6f, 0.0f };
		// �����p�x
		Vector2 factorAngleCos = { 0.2f,0.5f };
		// �L���t���O
		bool active = false;

	public: // �����o�֐�
		// setter
		inline void SetDir(const XMVECTOR& dir) { this->dir = DirectX::XMVector3Normalize(dir); }
		inline void SetCasterPos(const Vector3& casterpos) { casterPos = casterpos; }
		inline void SetDistanceCasterLight(float distancecasterlight) { distanceCasterLight = distancecasterlight; }
		inline void SetAtten(const Vector3& atten) { this->atten = atten; }
		inline void SetFactorAngle(const Vector2& factorangle) {
			factorAngleCos.x = cosf(General::ConvertToRadians(factorangle.x));
			factorAngleCos.y = cosf(General::ConvertToRadians(factorangle.y));
		}
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const XMVECTOR& GetDir() { return dir; }
		inline const Vector3& GetCasterPos() { return casterPos; }
		inline const float GetDistanceCasterLight() { return distanceCasterLight; }
		inline const Vector3& GetAtten() { return atten; }
		inline const Vector2& GetFactorAngleCos() { return factorAngleCos; }
		inline const bool IsActive() { return active; }
	};
}
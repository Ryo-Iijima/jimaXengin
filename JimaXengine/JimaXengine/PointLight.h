#pragma once
#include "math/Vector3.h"

namespace JimaXengine
{
	/// <summary>
	/// �|�C���g���C�g
	/// </summary>
	class PointLight
	{
	public: // �T�u�N���X
		// �萔�o�b�t�@�p�\����
		struct ConstBufferData
		{
			Vector3 lightPos;
			float pad1;
			Vector3 lightColor;
			float pad2;
			Vector3 lightAtten;
			unsigned int active;
		};

	private: // �����o�ϐ�
		// ���W
		Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
		// �F
		Vector3 lightColor = { 1.0f, 1.0f, 1.0f };
		// ���������W��
		Vector3 lightAtten = { 1.0f, 1.0f, 1.0f };
		// �L���t���O
		bool active = false;

	public: // �����o�֐�
		// setter
		inline void SetLightPos(const Vector3& lightpos) { lightPos = lightpos; }
		inline void SetLightColor(const Vector3& lightcolor) { lightColor = lightcolor; }
		inline void SetLightAtten(const Vector3& lightatten) { lightAtten = lightatten; }
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const Vector3& GetLightPos() { return lightPos; }
		inline const Vector3& GetLightColor() { return lightColor; }
		inline const Vector3& GetLightAtten() { return lightAtten; }
		inline const bool IsActive() { return active; }
	};
}
#pragma once
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

//�p�[�e�B�N���̃p�����[�^�\����
namespace JimaXengine
{
	struct ParticleParam
	{
		//�ʒu�A���x
		Vector3 pos = Vector3::Zero;
		Vector3 vel = Vector3::Zero;
		Vector3 acc = Vector3::Zero;
		float speed = 0.0f;
		//�傫��
		Vector3 scl = Vector3::One;
		Vector3 add_scl = Vector3::Zero;
		//��]
		Vector3 rot = Vector3::Zero;
		Vector3 add_rot = Vector3::Zero;
		//�̗�
		int life = 60;
		//�F�A�����x
		Vector3 color = Vector3::One;
		float alpha = 1.0f;
		float add_alpha = 0.0f;
	};
}
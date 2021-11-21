#pragma once
#include "camera/Camera.h"
#include <DirectXMath.h>
#include "math/Vector2.h"

namespace JimaXengine
{
	class DebugCamera :public Camera
	{
	private:
		// �}�E�X�ړ��ʂ̃X�P�[�����O
		Vector2 scale;

		// ���ʏ�̍��W�����߂�p
		Vector2 angle = { 0,0 };
		float r = 1;

		// target�ړ���
		Vector3 targetOffset = { 0,0,0 };


	public:
		DebugCamera();
		~DebugCamera();

		void Move()override;
	};
}
#pragma once
#include "camera/Camera.h"
#include <DirectXMath.h>
#include "math/Vector2.h"

namespace JimaXengine
{
	class DebugCamera :public Camera
	{
	private:
		// �ύX�t���O
		bool dirty = false;

		// ���N���b�N������p
		Vector2 currentPos;		
		Vector2 prevPos;
		Vector2 diff;

		// ���N���b�N������p
		float nScale = 50.0f;
		DirectX::XMMATRIX vMat;
		Vector3 X;
		Vector3 Y;

		// �}�E�X�ړ��ʂ̃X�P�[�����O
		Vector2 scale;

		// ���ʏ�̍��W�����߂�p
		Vector2 angle = { 0,0 };
		float r = 1;
		Vector3 lange;

		// target�ړ���
		Vector3 targetOffset = { 0,0,0 };


	public:
		DebugCamera();
		~DebugCamera();

		void SetInitialViewMatrix(const Vector3& argEye, const Vector3& argTarget) override;
		void Move() override;
	};
}
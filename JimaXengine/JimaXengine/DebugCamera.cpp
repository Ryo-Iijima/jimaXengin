#include "DebugCamera.h"
#include "general/Input.h"
#include "WinApp.h"
#include <stdio.h>
#include <cmath>
#include "general/General.h"

JimaXengine::DebugCamera::DebugCamera()
	:Camera()
{
	// ��ʃT�C�Y�ɑ΂��鑊�ΓI�ȃX�P�[���ɒ���
	scale.x = 1.0f / WinApp::WINDOW_WIDTH;
	scale.y = 1.0f / WinApp::WINDOW_HEIGHT;

	eye = { 0, -9, -23 };
	target = { 0, 1.5, 10 };

	angle.x = -90;
}

JimaXengine::DebugCamera::~DebugCamera()
{
}

void JimaXengine::DebugCamera::Move()
{
	bool dirty = false;
	targetOffset = { 0, 0, 0 };


	// �}�E�X�̍��{�^����������Ă�����J��������]������
	if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(DIMOFS_X))
	{
		Vector2 currentPos = Input::GetCurrentMousePos();
		Vector2 prevPos = Input::GetPrevMousePos();
		Vector2 diff = prevPos - currentPos;
		diff.y *= -1;

		// x=��,y=��
		angle += diff;

		dirty = true;
	}


	// �}�E�X�̒��{�^����������Ă�����J��������s�ړ�������
	if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(2))
	{
		float nScale = 50.0f;
		Vector2 currentPos = Input::GetCurrentMousePos();
		Vector2 prevPos = Input::GetPrevMousePos();
		Vector2 diff = prevPos - currentPos;
		diff /= nScale;
		targetOffset = { diff.x,-diff.y,0 };

		DirectX::XMMATRIX vMat = GetMatView();
		vMat = DirectX::XMMatrixInverse(0, vMat);
		
		Vector3 X = { vMat.r[0].m128_f32[0], vMat.r[0].m128_f32[1], vMat.r[0].m128_f32[2] };
		Vector3 Y = { vMat.r[1].m128_f32[0], vMat.r[1].m128_f32[1], vMat.r[1].m128_f32[2] };

		targetOffset = X * targetOffset.x + Y * targetOffset.y;

		dirty = true;
	}

	// �z�C�[�����͂ŋ�����ύX
	if (Input::MouseWheelMove() != 0) 
	{
		Vector3 lange = eye - target;

		r = abs(lange.Length());
		r += -Input::MouseWheelMove()/ 100.0f;

		dirty = true;
	}

	if (dirty) 
	{
		General:: ConvertToRadians(0);

		Vector3 newEye;
		eye.x = cosf(General::ConvertToRadians(angle.x)) * cosf(General::ConvertToRadians(angle.y));
		eye.z = sinf(General::ConvertToRadians(angle.x)) * cosf(General::ConvertToRadians(angle.y));
		eye.y = sinf(General::ConvertToRadians(angle.y));
		// ���a���|����
		eye *= r;
		// �����_�����炷
		target += targetOffset;
		// �����_�����炷
		eye += target;

		SetViewMatrix(eye, target, up);
	}
}
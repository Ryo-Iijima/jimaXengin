#include "DebugCamera.h"
#include "general/Input.h"
#include "WinApp.h"
#include <stdio.h>
#include <cmath>

JimaXengine::DebugCamera::DebugCamera()
	:Camera()
{
	// ��ʃT�C�Y�ɑ΂��鑊�ΓI�ȃX�P�[���ɒ���
	scale.x = 1.0f / WinApp::WINDOW_WIDTH;
	scale.y = 1.0f / WinApp::WINDOW_HEIGHT;
}

JimaXengine::DebugCamera::~DebugCamera()
{
}

void JimaXengine::DebugCamera::Move()
{
	bool dirty = false;
	targetOffset = { 0, 0, 0 };

	// view�s��̋t�s�񂪃J�����̃��[���h�s��

	// �}�E�X�̍��{�^����������Ă�����J��������]������
	if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(DIMOFS_X))
	{
		Vector2 currentPos = Input::GetCurrentMousePos();
		Vector2 prevPos = Input::GetPrevMousePos();
		Vector2 diff = prevPos - currentPos;
		diff *= scale;
		diff.y *= -1;

		// x=��,y=��
		angle += diff * DirectX::XM_PI;

		// -90<��<90 , 0<=��<=360
		if (angle.x > 90)
		{
			angle.x = 90;
		}
		if (angle.x < -90)
		{
			angle.x = -90;
		}
		if (angle.y > 360)
		{
			angle.y = 0;
		}
		if (angle.y < 0)
		{
			angle.y = 360;
		}

		dirty = true;
	}


	// �}�E�X�̒��{�^����������Ă�����J��������s�ړ�������
	if (Input::KeyPress(DIK_LALT) && (Input::KeyPress(DIK_LEFT) || Input::KeyPress(DIK_RIGHT) || Input::KeyPress(DIK_UP) || Input::KeyPress(DIK_DOWN)))
	//if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(DIMOFS_Z))
	{
		{// �L�[�{�[�h�ł�target����@���s�ړ��i�e�X�g�j
			float a = 0.5f;
			if (Input::KeyPress(DIK_LEFT)) targetOffset.x += a;
			if (Input::KeyPress(DIK_RIGHT)) targetOffset.x -= a;
			if (Input::KeyPress(DIK_UP)) targetOffset.y -= a;
			if (Input::KeyPress(DIK_DOWN)) targetOffset.y += a;
		}

		//{
		//	float nScale = 50.0f;
		//	Vector2 currentPos = Input::GetCurrentMousePos();
		//	Vector2 prevPos = Input::GetPrevMousePos();
		//	Vector2 diff = prevPos - currentPos;
		//	diff /= nScale;
		//	targetOffset = { diff.x,diff.y,0 };
		//}

		DirectX::XMMATRIX vMat = GetMatView();
		
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
		//r = max(r, 1.0f);

		dirty = true;
	}

	if (dirty) 
	{
		Vector3 newEye;
		eye.x = cosf(angle.x) * cosf(angle.y);
		eye.z = sinf(angle.x) * cosf(angle.y);
		eye.y = sinf(angle.y);
		// ���a���|����
		eye *= r;
		// �����_�����炷
		target += targetOffset;
		// �����_�����炷
		eye += target;

		SetViewMatrix(eye, target, up);
	}
}
#include "DebugCamera.h"
#include "general/Input.h"
#include "WinApp.h"
#include <stdio.h>
#include <cmath>

JimaXengine::DebugCamera::DebugCamera()
	:Camera()
{
	// 画面サイズに対する相対的なスケールに調整
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

	// view行列の逆行列がカメラのワールド行列

	// マウスの左ボタンが押されていたらカメラを回転させる
	if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(DIMOFS_X))
	{
		Vector2 currentPos = Input::GetCurrentMousePos();
		Vector2 prevPos = Input::GetPrevMousePos();
		Vector2 diff = prevPos - currentPos;
		diff *= scale;
		diff.y *= -1;

		// x=φ,y=θ
		angle += diff * DirectX::XM_PI;

		// -90<θ<90 , 0<=φ<=360
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


	// マウスの中ボタンが押されていたらカメラを並行移動させる
	if (Input::KeyPress(DIK_LALT) && (Input::KeyPress(DIK_LEFT) || Input::KeyPress(DIK_RIGHT) || Input::KeyPress(DIK_UP) || Input::KeyPress(DIK_DOWN)))
	//if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(DIMOFS_Z))
	{
		{// キーボードでのtarget操作　平行移動（テスト）
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

	// ホイール入力で距離を変更
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
		// 半径を掛ける
		eye *= r;
		// 注視点をずらす
		target += targetOffset;
		// 注視点分ずらす
		eye += target;

		SetViewMatrix(eye, target, up);
	}
}
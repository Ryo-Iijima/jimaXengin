#include "DebugCamera.h"
#include "general/Input.h"
#include "WinApp.h"
#include <stdio.h>
#include <cmath>
#include "general/General.h"

JimaXengine::DebugCamera::DebugCamera()
	:Camera()
{
	// 画面サイズに対する相対的なスケールに調整
	scale.x = 1.0f / WinApp::WINDOW_WIDTH;
	scale.y = 1.0f / WinApp::WINDOW_HEIGHT;

	eye = { 0, -9, -23 };
	target = { 0, 1.5, 10 };

	angle.x = -90;

	lange = eye - target;

	r = abs(lange.Length());
	r += -Input::MouseWheelMove() / 100.0f;

}

JimaXengine::DebugCamera::~DebugCamera()
{
}

void JimaXengine::DebugCamera::SetInitialViewMatrix(const Vector3& argEye, const Vector3& argTarget)
{
	eye = argEye;
	target = argTarget;

	lange = eye - target;
	r = abs(lange.Length());
	r += -Input::MouseWheelMove() / 100.0f;
}

void JimaXengine::DebugCamera::Move()
{
	dirty = false;
	targetOffset = { 0, 0, 0 };


	// マウスの左ボタンが押されていたらカメラを回転させる
	if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(DIMOFS_X))
	{
		currentPos = Input::GetCurrentMousePos();
		prevPos = Input::GetPrevMousePos();
		diff = prevPos - currentPos;
		diff.y *= -1;

		// x=φ,y=θ
		angle += diff;

		dirty = true;
	}


	// マウスの中ボタンが押されていたらカメラを並行移動させる
	if ((Input::KeyPress(DIK_LALT) || Input::KeyPress(DIK_RALT)) && Input::MouseButtonHold(2))
	{
		currentPos = Input::GetCurrentMousePos();
		prevPos = Input::GetPrevMousePos();
		diff = prevPos - currentPos;
		diff /= nScale;
		targetOffset = { diff.x,-diff.y,0 };

		vMat = GetMatView();
		vMat = DirectX::XMMatrixInverse(0, vMat);
		
		X = { vMat.r[0].m128_f32[0], vMat.r[0].m128_f32[1], vMat.r[0].m128_f32[2] };
		Y = { vMat.r[1].m128_f32[0], vMat.r[1].m128_f32[1], vMat.r[1].m128_f32[2] };

		targetOffset = X * targetOffset.x + Y * targetOffset.y;

		dirty = true;
	}

	// ホイール入力で距離を変更
	if (Input::MouseWheelMove() != 0) 
	{
		lange = eye - target;

		r = abs(lange.Length());
		r += -Input::MouseWheelMove()/ 100.0f;

		dirty = true;
	}

	if (dirty) 
	{
		eye.x = cosf(General::ConvertToRadians(angle.x)) * cosf(General::ConvertToRadians(angle.y));
		eye.z = sinf(General::ConvertToRadians(angle.x)) * cosf(General::ConvertToRadians(angle.y));
		eye.y = sinf(General::ConvertToRadians(angle.y));
		// 半径を掛ける
		eye *= r;
		// 注視点をずらす
		target += targetOffset;
		// 注視点分ずらす
		eye += target;

		SetViewMatrix(eye, target, up);
	}
}
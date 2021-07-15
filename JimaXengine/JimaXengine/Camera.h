#pragma once
#include <DirectXMath.h>
#include "math/Vector3.h"

class Camera
{
private:
	DirectX::XMMATRIX matProjection;	// 射影行列
	DirectX::XMMATRIX matView;			// ビュー行列
	Vector3 eye;						// 視点座標(カメラ位置)
	Vector3 target;						// 注視点座標
	Vector3 up;							// 上方向ベクトル
	DirectX::XMMATRIX matBillboard;		// 全方向ビルボード
	DirectX::XMMATRIX matBillboardY;	// Y軸ビルボード

	Vector3 rotationEye;
	Vector3 rotationTarget;	// カメラの回転角
public:
	Camera();
	~Camera();
	void SetProjectionMatrix(float win_width, float win_height, float angleOfView = 60.0f, float near = 0.1f, float far = 10000.0f);
	void SetViewMatrix(Vector3 argEye = { 0,0,-100 }, Vector3 argTarget = { 0,0,0 }, Vector3 argUp = { 0,1,0 });

	DirectX::XMMATRIX GetMatProjection() { return matProjection; }
	DirectX::XMMATRIX GetMatView() { return matView; }
	DirectX::XMMATRIX GetMatBillboard() { return matBillboard; }
	DirectX::XMMATRIX GetMatBillboardY() { return matBillboardY; }
	Vector3 GetEye() { return eye; }
	Vector3 GetTarget() { return target; }
	Vector3 GetUpVec() { return up; }
};
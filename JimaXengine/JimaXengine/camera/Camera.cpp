#include "Camera.h"

using namespace DirectX;

JimaXengine::Camera::Camera()
{
	matBillboard = XMMatrixIdentity();
	matBillboardY = XMMatrixIdentity();
}

JimaXengine::Camera::~Camera()
{
}

void JimaXengine::Camera::SetProjectionMatrix(float win_width, float win_height, float angleOfView, float near, float far)
{
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(angleOfView),		// 画角
		win_width / win_height,					// アスペクト比
		near, far);								// 前後
}

void JimaXengine::Camera::SetViewMatrix(Vector3 argEye, Vector3 argTarget, Vector3 argUp)
{
	// ビュー行列
	eye = argEye;
	target = argTarget;
	up = argUp;

	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	XMVECTOR upVector = XMLoadFloat3(&up);

	// カメラZ軸(視線方向)
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

	// 0ベクトルは除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	// ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// カメラのX軸(右方向)
	XMVECTOR cameraAxisX;
	// X軸は上方向→Z軸の外積から求める
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// カメラのY軸(上方向)
	XMVECTOR cameraAxisY;
	// Y軸はZ軸→X軸の外積から求める
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// カメラ回転行列
	XMMATRIX matCameraRot;
	// カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	// 転置(行と列が逆になる)により逆行列を計算
	matView = XMMatrixTranspose(matCameraRot);

	// 視点座標に-1をかけた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);

	// カメラの位置からワールド原点へのベクトル(カメラ座標系)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition); // X成分
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition); // Y成分
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition); // Z成分

	// 一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	// ビュー行列に平行移動成分を設定
	matView.r[3] = translation;

	// 全方向ビルボード行列の計算

	// ビルボード行列
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);

	// Y軸回りビルボード行列の計算
	// カメラX,Y,Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X軸は共通
	ybillCameraAxisX = cameraAxisX;

	// Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);

	// Z軸はX軸→Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Y軸回りビルボード行列
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);
}
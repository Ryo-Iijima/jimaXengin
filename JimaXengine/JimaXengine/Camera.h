#pragma once
#include <DirectXMath.h>
#include "math/Vector3.h"

class Camera
{
private:
	DirectX::XMMATRIX matProjection;	// �ˉe�s��
	DirectX::XMMATRIX matView;			// �r���[�s��
	Vector3 eye;						// ���_���W(�J�����ʒu)
	Vector3 target;						// �����_���W
	Vector3 up;							// ������x�N�g��
	DirectX::XMMATRIX matBillboard;		// �S�����r���{�[�h
	DirectX::XMMATRIX matBillboardY;	// Y���r���{�[�h

	Vector3 rotationEye;
	Vector3 rotationTarget;	// �J�����̉�]�p
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
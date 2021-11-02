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
		XMConvertToRadians(angleOfView),		// ��p
		win_width / win_height,					// �A�X�y�N�g��
		near, far);								// �O��
}

void JimaXengine::Camera::SetViewMatrix(Vector3 argEye, Vector3 argTarget, Vector3 argUp)
{
	// �r���[�s��
	eye = argEye;
	target = argTarget;
	up = argUp;

	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	XMVECTOR upVector = XMLoadFloat3(&up);

	// �J����Z��(��������)
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

	// 0�x�N�g���͏��O
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	// �x�N�g���𐳋K��
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// �J������X��(�E����)
	XMVECTOR cameraAxisX;
	// X���͏������Z���̊O�ς��狁�߂�
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// �x�N�g���𐳋K��
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// �J������Y��(�����)
	XMVECTOR cameraAxisY;
	// Y����Z����X���̊O�ς��狁�߂�
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// �J������]�s��
	XMMATRIX matCameraRot;
	// �J�������W�n�����[���h���W�n�̕ϊ��s��
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	// �]�u(�s�Ɨ񂪋t�ɂȂ�)�ɂ��t�s����v�Z
	matView = XMMatrixTranspose(matCameraRot);

	// ���_���W��-1�����������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);

	// �J�����̈ʒu���烏�[���h���_�ւ̃x�N�g��(�J�������W�n)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition); // X����
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition); // Y����
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition); // Z����

	// ��̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	// �r���[�s��ɕ��s�ړ�������ݒ�
	matView.r[3] = translation;

	// �S�����r���{�[�h�s��̌v�Z

	// �r���{�[�h�s��
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);

	// Y�����r���{�[�h�s��̌v�Z
	// �J����X,Y,Z��
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X���͋���
	ybillCameraAxisX = cameraAxisX;

	// Y���̓��[���h���W�n��Y��
	ybillCameraAxisY = XMVector3Normalize(upVector);

	// Z����X����Y���̊O�ςŋ��܂�
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Y�����r���{�[�h�s��
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);
}
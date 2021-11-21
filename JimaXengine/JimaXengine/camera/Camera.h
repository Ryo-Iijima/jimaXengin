#pragma once
#include <DirectXMath.h>
#include "../math/Vector3.h"

namespace JimaXengine
{
	class Camera
	{
	private:
		DirectX::XMMATRIX matProjection;	// �ˉe�s��
		DirectX::XMMATRIX matView;			// �r���[�s��
		DirectX::XMMATRIX matBillboard;		// �S�����r���{�[�h
		DirectX::XMMATRIX matBillboardY;	// Y���r���{�[�h

	protected:
		Vector3 eye;						// ���_���W(�J�����ʒu)
		Vector3 target;						// �����_���W
		Vector3 up;							// ������x�N�g��

	public:
		Camera();
		~Camera();

		virtual void Move();

		void SetProjectionMatrix(float win_width, float win_height, float angleOfView = 60.0f, float near = 0.1f, float far = 10000.0f);
		void SetViewMatrix(const Vector3& argEye = { 0,0,-100 }, const Vector3& argTarget = { 0,0,0 }, const Vector3& argUp = { 0,1,0 });
		void SetEye(const Vector3& argEye) { eye = argEye; }
		void SetTarget(const Vector3& argTarget) { target = argTarget; }
		void SetUp(const Vector3& argUp) { up = argUp; }

		DirectX::XMMATRIX GetMatProjection() { return matProjection; }
		DirectX::XMMATRIX GetMatView() { return matView; }
		DirectX::XMMATRIX GetMatBillboard() { return matBillboard; }
		DirectX::XMMATRIX GetMatBillboardY() { return matBillboardY; }
		Vector3 GetEye() { return eye; }
		Vector3 GetTarget() { return target; }
		Vector3 GetUpVec() { return up; }
	};
}
#pragma once
#include "camera/Camera.h"
#include <DirectXMath.h>
#include "math/Vector2.h"

namespace JimaXengine
{
	class DebugCamera :public Camera
	{
	private:
		// 変更フラグ
		bool dirty = false;

		// 左クリック時操作用
		Vector2 currentPos;		
		Vector2 prevPos;
		Vector2 diff;

		// 中クリック時操作用
		float nScale = 50.0f;
		DirectX::XMMATRIX vMat;
		Vector3 X;
		Vector3 Y;

		// マウス移動量のスケーリング
		Vector2 scale;

		// 球面上の座標を求める用
		Vector2 angle = { 0,0 };
		float r = 1;
		Vector3 lange;

		// target移動量
		Vector3 targetOffset = { 0,0,0 };


	public:
		DebugCamera();
		~DebugCamera();

		void SetInitialViewMatrix(const Vector3& argEye, const Vector3& argTarget) override;
		void Move() override;
	};
}
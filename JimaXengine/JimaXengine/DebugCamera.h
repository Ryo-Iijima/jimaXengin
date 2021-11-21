#pragma once
#include "camera/Camera.h"
#include <DirectXMath.h>
#include "math/Vector2.h"

namespace JimaXengine
{
	class DebugCamera :public Camera
	{
	private:
		// マウス移動量のスケーリング
		Vector2 scale;

		// 球面上の座標を求める用
		Vector2 angle = { 0,0 };
		float r = 1;

		// target移動量
		Vector3 targetOffset = { 0,0,0 };


	public:
		DebugCamera();
		~DebugCamera();

		void Move()override;
	};
}
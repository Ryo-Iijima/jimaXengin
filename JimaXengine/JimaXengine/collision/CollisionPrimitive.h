#pragma once
#include <DirectXMath.h>
#include "../math/Vector3.h"

namespace JimaXengine
{
	//球
	struct Sphere
	{
		//中心座標
		DirectX::XMVECTOR center = { 0,0,0,1 };
		//半径
		float radius = 1.0f;
	};

	//平面
	struct Plane
	{
		//法線ベクトル
		DirectX::XMVECTOR normal = { 0,1,0,0 };
		//原点(0,0,0)からの距離
		float distance = 0.0f;
	};

	//法線付き三角形(時計回りが表面)
	struct Triangle
	{
		//頂点座標3つ
		DirectX::XMVECTOR p0;
		DirectX::XMVECTOR p1;
		DirectX::XMVECTOR p2;

		//法線ベクトル
		DirectX::XMVECTOR normal = { 0,1,0,0 };

		//法線の計算
		void ComputeNormal();
	};

	//レイ(半直線)
	struct Lay
	{
		//始点座標
		DirectX::XMVECTOR start = { 0,0,0,1 };
		//方向
		DirectX::XMVECTOR dir = { 1,0,0,0 };
	};

	//軸平行バウンディングボックス(3D)
	struct AABB3D
	{
		//最小座標
		Vector3 minPos;
		//最大座標
		Vector3 maxPos;
	};

	//カプセル
	struct Capsule
	{
		Vector3 startPos;
		Vector3 endPos;
		float radius;
	};
}
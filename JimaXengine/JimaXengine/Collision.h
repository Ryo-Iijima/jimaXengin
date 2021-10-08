#pragma once

#include "CollisionPrimitive.h"

/// <summary>
/// 当たり判定ヘルパークラス
/// </summary>
class Collision
{
public:
	// 球と球の当たり判定
	static bool CheckSphereToSphere(const Sphere& sphere1, const Sphere& sphere2);

	// 球と平面の当たり判定
	static bool CheckSphereToPlane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR *inter = nullptr);
	
	// 点と三角形の最近接点を求める
	static void ClosestPtPointToTriangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest);

	// 球と法線付き三角形の当たり判定
	static bool CheckSphereToTriangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr);

	// レイと平面の当たり判定
	static bool CheckLayToPlane(const Lay& lay, const Plane& plane, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
	
	// レイと法線付き三角形の当たり判定
	static bool CheckLayToTriangle(const Lay& lay, const Triangle& triangle, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	// レイと球の当たり判定
	static bool CheckLayToSphere(const Lay& lay, const Sphere& sphere, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	// AABB3DとAABB3Dの当たり判定
	static bool CheckAABB3DToAABB3D(const AABB3D& box1, const AABB3D& box2);

	// AABB3Dと球の当たり判定
	static bool CheckAABB3DToSphere(const AABB3D& box, const Sphere sphere);

	// カプセルと球の当たり判定
	static bool CapsuleToSphere(const Sphere& sphere, const Capsule& capsule);

	// カプセルとカプセルの当たり判定
	static bool CapsuleToCapsule(const Capsule& capsule1, const Capsule& capsule2);

	// 線分と線分の距離の2乗を求める
	static float SqDistanceSegmentToSegment(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2);

	// 線分とAABBの当たり判定
	static bool LineToAABB3D(struct Lay* lay, AABB3D* aabb, DirectX::XMMATRIX* mat, float& t, Vector3* colPos = 0);

	// 値を範囲に収める
	static float Clamp(float x, float low, float high);
};
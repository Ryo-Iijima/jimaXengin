#include "Collision.h"

using namespace DirectX;

bool Collision::CheckSphereToSphere(const Sphere & sphere1, const Sphere & sphere2)
{
	float x = sphere2.center.m128_f32[0] - sphere1.center.m128_f32[0];
	float y = sphere2.center.m128_f32[1] - sphere1.center.m128_f32[1];
	float z = sphere2.center.m128_f32[2] - sphere1.center.m128_f32[2];
	float r = sphere1.radius + sphere2.radius;
	
	if (!((x * x + y * y + z * z) <= r * r))
	{
		return false;
	}

	return true;
}

bool Collision::CheckSphereToPlane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter)
{
	// 座標系の原点から球の中心座標への距離
	XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);	

	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = distV.m128_f32[0] - plane.distance;

	// 距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > sphere.radius)	return false;

	// 擬似交点を計算
	if (inter)
	{
		// 平面上の最接近点を、疑似交点とする
		*inter = -dist * plane.normal + sphere.center;
	}

	return true;
}

void Collision::ClosestPtPointToTriangle(const DirectX::XMVECTOR & point, const Triangle & triangle, DirectX::XMVECTOR * closest)
{
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	XMVECTOR p0_pt = point - triangle.p0;

	XMVECTOR d1 = XMVector3Dot(p0_p1, p0_pt);
	XMVECTOR d2 = XMVector3Dot(p0_p2, p0_pt);

	if (d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f)
	{
		// p0が最近傍
		*closest = triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p1_pt = point - triangle.p1;

	XMVECTOR d3 = XMVector3Dot(p0_p1, p1_pt);
	XMVECTOR d4 = XMVector3Dot(p0_p2, p1_pt);

	if (d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0])
	{
		// p1が最近傍
		*closest = triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];
	if (vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f)
	{
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p2_pt = point - triangle.p2;

	XMVECTOR d5 = XMVector3Dot(p0_p1, p2_pt);
	XMVECTOR d6 = XMVector3Dot(p0_p2, p2_pt);
	if (d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0])
	{
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];
	if (vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f)
	{
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3.m128_f32[0] * d6.m128_f32[0] - d5.m128_f32[0] * d4.m128_f32[0];
	if (va <= 0.0f && (d4.m128_f32[0] - d3.m128_f32[0]) >= 0.0f && (d5.m128_f32[0] - d6.m128_f32[0]) >= 0.0f)
	{
		float w = (d4.m128_f32[0] - d3.m128_f32[0]) / ((d4.m128_f32[0] - d3.m128_f32[0]) + (d5.m128_f32[0] - d6.m128_f32[0]));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphereToTriangle(const Sphere & sphere, const Triangle & triangle, DirectX::XMVECTOR * inter)
{
	XMVECTOR p;
	//弾の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPointToTriangle(sphere.center, triangle, &p);

	//点pと球の中心の差分ベクトル
	XMVECTOR v = p - sphere.center;

	//距離の二乗を求める
	//同じベクトル同士の内積は三平方の定理のルート内部の式と一致する
	v = XMVector3Dot(v, v);

	//弾と三角形の距離が半径以下なら当たっていない
	if (v.m128_f32[0] > sphere.radius * sphere.radius)return false;

	//擬似交点を計算
	if (inter)
	{
		//三角形増の最近接点pを擬似交点とする
		*inter = p;
	}
	return true;
}

bool Collision::CheckLayToPlane(const Lay & lay, const Plane & plane, float * distance, DirectX::XMVECTOR * inter)
{
	const float epsilon = 1.0e-5f;	//誤差吸収用の微小な値

	//面法線とレイの方向ベクトルの内積
	float d1 = XMVector3Dot(plane.normal, lay.dir).m128_f32[0];

	//裏面には当たらない
	if (d1 > -epsilon) { return false; }

	//始点と原点の距離(平面の法線方向)
	//面法線とレイの始点座標(位置ベクトル)の内積
	float d2 = XMVector3Dot(plane.normal, lay.start).m128_f32[0];

	//始点と平面の距離(平面の法線方向)
	float dist = d2 - plane.distance;

	//始点と平面の距離(レイ方向)
	float t = dist / -d1;

	//交点が始点より後ろにあるので当たらない
	if (t < 0)return false;

	//距離を書き込む
	if (distance) { *distance = t; }

	//交点を計算
	if (inter) { *inter = lay.start + t * lay.dir; }

	return true;
}

bool Collision::CheckLayToTriangle(const Lay & lay, const Triangle & triangle, float * distance, DirectX::XMVECTOR * inter)
{
	//三角形が乗っている平面を算出
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];

	//レイと平面が当たっていなければ、当たっていない
	if (!CheckLayToPlane(lay, plane, distance, &interPlane)) { return false; }

	//レイと平面が当たっていたので、距離と交点が書き込まれた
	//レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f; //誤差吸収用の微小な値
	XMVECTOR m;

	//辺p0_p1について
	XMVECTOR pt_p0 = triangle.p0 - interPlane;
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);

	//辺の外側であれば当たっていないので判定を打ち切る
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) { return false; }

	//辺p1_p2について
	XMVECTOR pt_p1 = triangle.p1 - interPlane;
	XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);

	//辺の外側であれば当たっていないので判定を打ち切る
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) { return false; }

	//辺p2_p0について
	XMVECTOR pt_p2 = triangle.p2 - interPlane;
	XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);

	//辺の外側であれば当たっていないので判定を打ち切る
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) { return false; }

	//内側なので当たっている
	if (inter) { *inter = interPlane; }

	return true;
}

bool Collision::CheckLayToSphere(const Lay & lay, const Sphere & sphere, float * distance, DirectX::XMVECTOR * inter)
{
	XMVECTOR m = lay.start - sphere.center;
	float b = XMVector3Dot(m, lay.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;

	//layの始点がsphereの外側にあり(c > 0)、layがsphereから離れていく方向を指している場合(b > 0)、当たらない
	if (c > 0.0f &&b > 0.0f)return false;

	float discr = b * b - c;
	//負の判別式はレイが球を外れていることに一致
	if (discr < 0.0f) { return false; }

	//レイは球と交差している
	//交差する最小の値tを計算
	float t = -b - sqrtf(discr);

	//tが負である場合、レイは球の内側から開始しているのでtをゼロにクランプ
	if (t < 0)t = 0.0f;
	if (distance) { *distance = t; }
	if (inter) { *inter = lay.start + t * lay.dir; }

	return true;
}

bool Collision::CheckAABB3DToAABB3D(const AABB3D & box1, const AABB3D & box2)
{
	//X
	if (box1.minPos.x > box2.maxPos.x) return false;
	if (box1.maxPos.x < box2.minPos.x) return false;
	//Y
	if (box1.minPos.y > box2.maxPos.y) return false;
	if (box1.maxPos.y < box2.minPos.y) return false;
	//Z
	if (box1.minPos.z > box2.maxPos.z) return false;
	if (box1.maxPos.z < box2.minPos.z) return false;

	return true;
}

bool Collision::CheckAABB3DToSphere(const AABB3D & box, const Sphere sphere)
{
	float distance = 0.0f;
	float d;

	d = sphere.center.m128_f32[0];
	if (d < box.minPos.x)
	{
		distance += (box.minPos.x - d) * (box.minPos.x - d);
	}
	if (d > box.maxPos.x)
	{
		distance += (box.maxPos.x - d) * (box.maxPos.x - d);
	}

	d = sphere.center.m128_f32[1];
	if (d < box.minPos.y)
	{
		distance += (box.minPos.y - d) * (box.minPos.y - d);
	}
	if (d > box.maxPos.y)
	{
		distance += (box.maxPos.y - d) * (box.maxPos.y - d);
	}

	d = sphere.center.m128_f32[2];
	if (d < box.minPos.z)
	{
		distance += (box.minPos.z - d) * (box.minPos.z - d);
	}
	if (d > box.maxPos.z)
	{
		distance += (box.maxPos.z - d) * (box.maxPos.z - d);
	}

	return (distance < (sphere.radius * sphere.radius));
}

bool Collision::CapsuleToSphere(const Sphere & sphere, const Capsule & capsule)
{
	//カプセルのスタートからエンドまでのベクトル
	Vector3 vStartToEnd = capsule.endPos - capsule.startPos;

	Vector3 n = vStartToEnd;
	n.Normalize();

	//球の中心座標の取り出し
	Vector3 spherePos;
	XMStoreFloat3(&spherePos, sphere.center);

	//カプセルのスタートからスフィアへのベクトル
	Vector3 pspc = spherePos - capsule.startPos;
	float t = n.Dot(pspc);

	//カプセルのスタートからスフィアから線分に垂直に下した点へのベクトル
	Vector3 pspn = n * t;
	Vector3 pospn = capsule.startPos + pspn;

	//比率
	float lengthRate = t / vStartToEnd.Length();

	float distance;
	if (lengthRate < 0.0f)
	{
		//スフィアがカプセルのスタートより手前
		distance = pspc.Length() - capsule.radius;
	}
	else if (lengthRate <= 1.0f)
	{
		//スフィアがカプセルのスタートからエンドまでの間
		Vector3 pnpc = pospn - spherePos;
		distance = pnpc.Length() - capsule.radius;
	}
	else
	{
		//スフィアがカプセルのエンドより奥
		Vector3 pepc = spherePos - capsule.endPos;
		distance = pepc.Length() - capsule.radius;
	}

	return distance < sphere.radius;
}

bool Collision::CapsuleToCapsule(const Capsule & capsule1, const Capsule & capsule2)
{
	//線分と線分の距離を調べる(結果は距離の2乗)
	float sqDistance =
		SqDistanceSegmentToSegment(capsule1.startPos, capsule1.endPos, capsule2.startPos, capsule2.endPos);

	//2つのカプセルの半径の和を求める
	float r = capsule1.radius + capsule2.radius;

	//衝突判定の結果を返す
	return (sqDistance < (r * r));
}

float Collision::SqDistanceSegmentToSegment(const Vector3 & p1, const Vector3 & q1, const Vector3 & p2, const Vector3 & q2)
{
	//p1->q1のベクトル
	Vector3 d1 = q1 - p1;
	//p2->q2のベクトル
	Vector3 d2 = q2 - p2;

	Vector3 r = p1 - p2;

	float a = d1.Dot(d1);
	float b = d1.Dot(d2);
	float e = d2.Dot(d2);

	float c = d1.Dot(r);
	float f = d2.Dot(r);

	float s = 0.0f;
	float t = 0.0f;

	float denominetor = a * e - (b * b);

	//sの値を求める
	if (denominetor != 0.0f) //2つの線分をが平行でないなら
	{
		s = ((b * f) - (c * e)) / denominetor;
		s = Clamp(s, 0.0f, 1.0f);
	}
	else //2つの線分が平行であるとき
	{
		s = 0.0f;
	}

	//tの値を求める
	//t = ((p1 + d1 * s) - p2) * d2 / (d2 /d2)
	t = ((a * f) - (b * c)) / denominetor;

	//tが0.0f~1.0fの範囲外であればsを再計算する
	//s = ((p2 + d2 * t) - p1) * d1 / (d1 * d1) = (t * b - c) / a
	if (t < 0.0f)
	{
		t = 0.0f;
		s = Clamp(-c / a, 0.0f, 1.0f);
	}
	else if (t > 1.0f)
	{
		s = Clamp((b - c) / a, 0.0f, 1.0f);
		t = 1.0f;
	}

	//sとtの値が決定したので各線分内の座標c1とc2を求める
	Vector3 c1 = p1 + (d1 * s);
	Vector3 c2 = p2 + (d2 * t);

	//2点間(c1とc2)の距離の2乗を求めて結果を返す
	Vector3 tmp = c1 - c2;

	return (tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
}

float Collision::Clamp(float x, float low, float high)
{
	x = (x < low) ? low : x;
	x = (x > high) ? high : x;
	return x;
}

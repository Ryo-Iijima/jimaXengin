#pragma once
#include <DirectXMath.h>

namespace JimaXengine
{
	struct Vector3 : public DirectX::XMFLOAT3
	{
		Vector3();
		Vector3(float x, float y, float z);

		float Length() const;
		Vector3& Normalize();
		float Dot(const Vector3& v)const;
		Vector3 Cross(const Vector3& v)const;
		DirectX::XMVECTOR ConvertXMVECTOR();

		static Vector3 Lerp(const Vector3& a, const Vector3& b, float time);
		static const Vector3 Zero;
		static const Vector3 One;
		static float Distance(const Vector3& a, const Vector3& b);

		Vector3 operator+() const;
		Vector3 operator-() const;

		Vector3 operator+(const Vector3& v) const;
		Vector3 operator-(const Vector3& v) const;
		Vector3 operator*(const float scalar) const;
		Vector3 operator/(const float scalar) const;

		Vector3 operator*(const DirectX::XMMATRIX& v) const;

		Vector3 operator+=(const Vector3& v);
		Vector3 operator-=(const Vector3& v);
		Vector3 operator*=(float s);
		Vector3 operator/=(float s);

		Vector3 operator*=(const DirectX::XMMATRIX& v);

		bool operator==(const Vector3& v);
		bool operator!=(const Vector3& v);
	};
}
#pragma once
#include <DirectXMath.h>

namespace JimaXengine
{
	struct Vector2 : public DirectX::XMFLOAT2
	{
		Vector2();
		Vector2(float x, float y);

		float Length() const;
		Vector2& Normalize();
		float Dot(const Vector2& other) const;
		float Cross(const Vector2& other) const;

		Vector2 operator+()const;
		Vector2 operator-()const;

		Vector2 operator +(const Vector2& v) const;
		Vector2 operator -(const Vector2& v) const;
		Vector2 operator *(const Vector2& v) const;
		Vector2 operator *(const float scalar) const;
		Vector2 operator /(const float scalar) const;

		Vector2& operator +=(const Vector2& other);
		Vector2& operator -=(const Vector2& other);
		Vector2& operator *=(const Vector2& other);
		Vector2& operator *=(float scalar);
		Vector2& operator /=(float scalar);
	};
}
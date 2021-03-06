#pragma once
#include "Vector3.h"

namespace JimaXengine
{
	struct Vector4
	{
		float x;
		float y;
		float z;
		float w;

		Vector4();
		Vector4(float x, float y, float z, float w);
		Vector4(Vector3 color, float alpha);

		Vector4 operator+() const;
		Vector4 operator-() const;

		Vector4 operator+(const Vector4& v) const;
		Vector4 operator-(const Vector4& v) const;
		Vector4 operator*(const float scalar) const;
		Vector4 operator/(const float scalar) const;

		Vector4 operator+=(const Vector4& v);
		Vector4 operator-=(const Vector4& v);
		Vector4 operator*=(float scalar);
		Vector4 operator/=(float scalar);
	};
}
#include "Vector2.h"

Vector2::Vector2() : DirectX::XMFLOAT2(0.0f, 0.0f)
{
}

Vector2::Vector2(float x, float y) : DirectX::XMFLOAT2(x, y)
{
}

float Vector2::Length() const
{
	return sqrtf(x * x + y * y);
}

Vector2& Vector2::Normalize()
{
	float len = Length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float Vector2::Dot(const Vector2 & other) const
{
	return x * other.x + y * other.y;
}

float Vector2::Cross(const Vector2 & other) const
{
	return x * other.y - y * other.x;
}

Vector2 Vector2::operator+() const
{
	return *this;
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator+(const Vector2 & v) const
{
	return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator-(const Vector2 & v) const
{
	return Vector2(x - v.x, y - v.y);
}

Vector2 Vector2::operator*(const float scalar) const
{
	return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(const float scalar) const
{
	return Vector2(x / scalar, y / scalar);
}

Vector2 & Vector2::operator+=(const Vector2 & other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vector2 & Vector2::operator-=(const Vector2 & other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2 & Vector2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

Vector2 & Vector2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}
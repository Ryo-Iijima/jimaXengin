#include "Vector2.h"

JimaXengine::Vector2::Vector2() : DirectX::XMFLOAT2(0.0f, 0.0f)
{
}

JimaXengine::Vector2::Vector2(float x, float y) : DirectX::XMFLOAT2(x, y)
{
}

float JimaXengine::Vector2::Length() const
{
	return sqrtf(x * x + y * y);
}

JimaXengine::Vector2& JimaXengine::Vector2::Normalize()
{
	float len = Length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float JimaXengine::Vector2::Dot(const Vector2 & other) const
{
	return x * other.x + y * other.y;
}

float JimaXengine::Vector2::Cross(const Vector2 & other) const
{
	return x * other.y - y * other.x;
}

JimaXengine::Vector2 JimaXengine::Vector2::operator+() const
{
	return *this;
}

JimaXengine::Vector2 JimaXengine::Vector2::operator-() const
{
	return Vector2(-x, -y);
}

JimaXengine::Vector2 JimaXengine::Vector2::operator+(const Vector2 & v) const
{
	return Vector2(x + v.x, y + v.y);
}

JimaXengine::Vector2 JimaXengine::Vector2::operator-(const Vector2 & v) const
{
	return Vector2(x - v.x, y - v.y);
}

JimaXengine::Vector2 JimaXengine::Vector2::operator*(const float scalar) const
{
	return Vector2(x * scalar, y * scalar);
}

JimaXengine::Vector2 JimaXengine::Vector2::operator/(const float scalar) const
{
	return Vector2(x / scalar, y / scalar);
}

JimaXengine::Vector2 & JimaXengine::Vector2::operator+=(const Vector2 & other)
{
	x += other.x;
	y += other.y;
	return *this;
}

JimaXengine::Vector2 & JimaXengine::Vector2::operator-=(const Vector2 & other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

JimaXengine::Vector2& JimaXengine::Vector2::operator*=(const Vector2& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}

JimaXengine::Vector2 & JimaXengine::Vector2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

JimaXengine::Vector2 & JimaXengine::Vector2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}
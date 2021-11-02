#include "Vector4.h"

JimaXengine::Vector4::Vector4()
	: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

JimaXengine::Vector4::Vector4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{
}

JimaXengine::Vector4 JimaXengine::Vector4::operator+() const
{
	return *this;
}

JimaXengine::Vector4 JimaXengine::Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

JimaXengine::Vector4 JimaXengine::Vector4::operator+(const Vector4 & v) const
{
	return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

JimaXengine::Vector4 JimaXengine::Vector4::operator-(const Vector4 & v) const
{
	return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

JimaXengine::Vector4 JimaXengine::Vector4::operator*(const float scalar) const
{
	return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

JimaXengine::Vector4 JimaXengine::Vector4::operator/(const float scalar) const
{
	return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

JimaXengine::Vector4 JimaXengine::Vector4::operator+=(const Vector4 & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

JimaXengine::Vector4 JimaXengine::Vector4::operator-=(const Vector4 & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

JimaXengine::Vector4 JimaXengine::Vector4::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

JimaXengine::Vector4 JimaXengine::Vector4::operator/=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

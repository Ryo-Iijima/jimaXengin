#include "Vector3.h"

JimaXengine::Vector3::Vector3() : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
{
}

JimaXengine::Vector3::Vector3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z)
{
}

const JimaXengine::Vector3 JimaXengine::Vector3::Zero = { 0, 0, 0 };
const JimaXengine::Vector3 JimaXengine::Vector3::One = { 1, 1, 1 };

float JimaXengine::Vector3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

JimaXengine::Vector3& JimaXengine::Vector3::Normalize()
{
	float len = Length();
	if (len != 0)
	{
		*this /= len;
		return *this;
	}
	return *this;
}

float JimaXengine::Vector3::Dot(const Vector3 & v) const
{
	return x * v.x + y * v.y + z * v.z;
}

JimaXengine::Vector3 JimaXengine::Vector3::Cross(const Vector3 & v) const
{
	Vector3 tmp;

	tmp.x = (y * v.z) - (z * v.y);
	tmp.y = (z * v.x) - (x * v.z);
	tmp.z = (x * v.y) - (y * v.x);

	return tmp;
}

DirectX::XMVECTOR JimaXengine::Vector3::ConvertXMVECTOR()
{
	return DirectX::XMLoadFloat3(this);
}

JimaXengine::Vector3 JimaXengine::Vector3::Lerp(const Vector3& start, const Vector3& end, const float time)
{
	Vector3 distance = end - start;
	Vector3 result = start + (distance * time);
	return result;
}

float JimaXengine::Vector3::Distance(const Vector3& a, const Vector3& b)
{
	float x = a.x - b.x;
	float y = a.y - b.y;
	float z = a.z - b.z;
	float distance = sqrtf(x * x + y * y + z * z);

	return distance;
}

JimaXengine::Vector3 JimaXengine::Vector3::operator+() const
{
	return *this;
}

JimaXengine::Vector3 JimaXengine::Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

JimaXengine::Vector3 JimaXengine::Vector3::operator+(const Vector3 & v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

JimaXengine::Vector3 JimaXengine::Vector3::operator-(const Vector3 & v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

JimaXengine::Vector3 JimaXengine::Vector3::operator*(const float scalar) const
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

JimaXengine::Vector3 JimaXengine::Vector3::operator/(const float scalar) const
{
	return Vector3(x / scalar, y / scalar, z / scalar);
}

JimaXengine::Vector3 JimaXengine::Vector3::operator+=(const Vector3 & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

JimaXengine::Vector3 JimaXengine::Vector3::operator-=(const Vector3 & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

JimaXengine::Vector3 JimaXengine::Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

JimaXengine::Vector3 JimaXengine::Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

bool JimaXengine::Vector3::operator==(const Vector3& v)
{
	if (x == v.x && y == v.y && z == v.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool JimaXengine::Vector3::operator!=(const Vector3& v)
{
	if (!(*this == v))
	{
		return true;
	}
	else
	{
		return false;
	}
}

#include "Vector3.h"

Vector3::Vector3() : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
{
}

Vector3::Vector3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z)
{
}

const Vector3 Vector3::Zero = { 0, 0, 0 };
const Vector3 Vector3::One = { 1, 1, 1 };

float Vector3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

Vector3& Vector3::Normalize()
{
	float len = Length();
	if (len != 0)
	{
		*this /= len;
		return *this;
	}
	return *this;
}

float Vector3::Dot(const Vector3 & v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross(const Vector3 & v) const
{
	Vector3 tmp;

	tmp.x = (y * v.z) - (z * v.y);
	tmp.y = (z * v.x) - (x * v.z);
	tmp.z = (x * v.y) - (y * v.x);

	return tmp;
}

DirectX::XMVECTOR Vector3::ConvertXMVECTOR()
{
	return DirectX::XMLoadFloat3(this);
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, const float time)
{
	Vector3 distance = end - start;
	Vector3 result = start + (distance * time);
	return result;
}

float Vector3::Distance(const Vector3& a, const Vector3& b)
{
	float x = a.x - b.x;
	float y = a.y - b.y;
	float z = a.z - b.z;
	float distance = sqrtf(x * x + y * y + z * z);

	return distance;
}

Vector3 Vector3::operator+() const
{
	return *this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator+(const Vector3 & v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator-(const Vector3 & v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator*(const float scalar) const
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(const float scalar) const
{
	return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3 Vector3::operator+=(const Vector3 & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3 Vector3::operator-=(const Vector3 & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3 Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3 Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

bool Vector3::operator==(const Vector3& v)
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

bool Vector3::operator!=(const Vector3& v)
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

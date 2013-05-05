#include "Vec3.h"

namespace GLib
{

Vec3::Vec3(float x, float y, float z)
	: D3DXVECTOR3(x, y, z)
{

}

Vec3::Vec3(const Vec3& vector)
	: D3DXVECTOR3(vector)
{

}

//Vec3(const XMFLOAT3& vector)
//	: Vec3(vector.x, vector.y, vector.z)
//{
//
//}

float Vec3::Dot(const Vec3& vector)
{
	return this->x * vector.x + this->y * vector.y + this->z * vector.z;
}

Vec3 Vec3::Cross(const Vec3& vector)
{
	Vec3 cross;

	cross.x = y * vector.z - z * vector.y;
	cross.y = z * vector.x - x * vector.z;
	cross.z = x * vector.y - y * vector.x;

	return cross;
}

void Vec3::Normalize()
{
	float length = Length();
	x /= length;
	y /= length;
	z /= length;
}

float Vec3::Length()
{
	return sqrt(x*x + y*y + z*z);
}

Vec3 Vec3::operator+(const Vec3& vector)
{
	return Vec3(x + vector.x, y + vector.y, z + vector.z);
}

void Vec3::operator=(const D3DXVECTOR3& b)
{
	x = b.x;
	y = b.y;
	z = b.z;
}

}
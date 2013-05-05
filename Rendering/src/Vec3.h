#pragma once

#include "GlibStd.h"
#include "Utility.h"

using namespace XNA;



namespace GLib {

class Vec3 : public D3DXVECTOR3
{
public:
	Vec3() : D3DXVECTOR3() {}
	Vec3(float x, float y, float z);
	Vec3(const Vec3& vector);
	//Vec3(const XMFLOAT3& vector);

	float Dot(const Vec3& vector);
	Vec3 Cross(const Vec3& vector);
	void Normalize();
	float Length();

public:
	Vec3 operator+(const Vec3& vector);
	void operator=(const D3DXVECTOR3& b);
};

}
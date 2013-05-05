//#include "Vec4.h"
//
//namespace GLib
//{
//
//	Vec4::Vec4(float x, float y, float z, float w)
//		: D3DXVECTOR4(x, y, z, w)
//	{
//
//	}
//
//	Vec4::Vec4(const Vec4& vector)
//		: D3DXVECTOR4(vector)
//	{
//
//	}
//
//	float Vec4::Dot(const Vec4& vector)
//	{
//		return x * vector.x + y * vector.y + z * vector.z + w * vector.w;
//	}
//
//	void Vec4::Normalize()
//	{
//		float length = Length();
//		x /= length;
//		y /= length;
//		z /= length;
//		w /= length;
//	}
//
//	float Vec4::Length()
//	{
//		return sqrt(x*x + y*y + z*z + w*w);
//	}
//
//}
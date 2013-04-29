#pragma once

/******************************************************************************************//**
* @file
* @brief Contains convenient functions and structures that are used frequently.
*********************************************************************************************/

#pragma warning( push )
#pragma warning (disable : 4005)

#include "GlibStd.h"
#include "xnacollision.h"
#include <D3DX10math.h>
#include "defs.h"
#include <string>
#include <vector>

#pragma warning( pop ) 

using namespace std;

class Object3D;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Light;
	class Graphics;
	class Camera;
	class Runnable;

	float GLIB_API			GetRandomFloat(float a, float b);
	wstring GLIB_API		StringToWString(const std::string &s);
	unsigned int GLIB_API	ColorRGBA(byte red, byte green, byte blue, byte alpha);
	vector<string> GLIB_API SplitString(string text, char delim);
	bool GLIB_API			IsNumber(const std::string& s);

	//! Takes a "0 0 0 255" string and splits it up to a UIN32 color value.
	UINT32 StripRGBA(string colors);

	struct Ray
	{
		XMFLOAT3 origin;
		XMFLOAT3 direction;
	};

	struct Dimension
	{
		Dimension(){};
		Dimension(float w, float h) {
			width = w;
			height = h;
		}

		float width;
		float height;
	};

	struct Rect
	{
		Rect() {}
		Rect(float l, float r, float t, float b) {
			left = l;
			right = r;
			top = t;
			bottom = b;
		}

		float Width() {
			return right - left;
		}

		float Height() {
			return bottom - top;
		}

		float left;
		float right;
		float top;
		float bottom;
	};

	struct BoundingSphere
	{
		BoundingSphere() : center(0.0f, 0.0f, 0.0f), radius(0.0f) {}
		XMFLOAT3 center;
		float radius;
	};

	//! Creates a texture array.
	ID3D11ShaderResourceView* CreateTexture2DArraySRV(ID3D11Device* device, ID3D11DeviceContext* context, vector<string>& filenames);

	//! Typedef for convenience
	typedef std::vector<Light*> LightList;

	//! Returns the inverse transpose.
	XMMATRIX GLIB_API InverseTranspose(CXMMATRIX M);

	//! Transform an axis aligned box by an angle preserving transform. Custom version of the XNA::TransformAxisAlignedBox.
	VOID TransformAxisAlignedBoxCustom(XNA::AxisAlignedBox* pOut, const XNA::AxisAlignedBox* pIn, FXMVECTOR Scale, FXMVECTOR Rotation, FXMVECTOR Translation);

	/******************************************************************************************//**
	*  Convenient storage of often used colors.
	*********************************************************************************************/
	namespace Colors
	{
		GLOBALCONST XMFLOAT4 White(1.0f, 1.0f, 1.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Black(0.0f, 0.0f, 0.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Red(1.0f, 0.0f, 0.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Green(0.0f, 1.0f, 0.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Blue(0.0f, 0.0f, 1.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Yellow(1.0f, 1.0f, 0.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Cyan(0.0f, 1.0f, 1.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Magenta(1.0f, 0.0f, 1.0f, 1.0f);
		GLOBALCONST XMFLOAT4 Silver(0.75f, 0.75f, 0.75f, 1.0f);
		GLOBALCONST XMFLOAT4 LightSteelBlue(0.69f, 0.77f, 0.87f, 1.0f);
	}
}

//
// Overloaded operators to make things smoother.
//
XMFLOAT3 operator+(const XMFLOAT3 a, const XMFLOAT3 b);
XMFLOAT3 operator-(const XMFLOAT3 a, const XMFLOAT3 b);

XMFLOAT3 operator+=(const XMFLOAT3 a, const XMVECTOR b);
XMFLOAT3 operator-=(const XMFLOAT3 a, const XMVECTOR b);

XMFLOAT2 operator+(const XMFLOAT2 a, const XMFLOAT2 b);
XMFLOAT2 operator-(const XMFLOAT2 a, const XMFLOAT2 b);

XMFLOAT4 operator*(const XMFLOAT4 a, const float b);
XMFLOAT3 operator*(const XMFLOAT3 a, const float b);
XMFLOAT3 operator*(const float b, const XMFLOAT3 a);

XMFLOAT3 operator+(const XMFLOAT3 a, const float b);
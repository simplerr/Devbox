#pragma warning( push )
#pragma warning (disable : 4005)

#include "Utility.h"
#include "GlibStd.h"
#include "Runnable.h"
#include <sstream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <xnamath.h>
#include "xnacollision.h"
using namespace std;

#pragma warning( pop ) 

//! Graphics Library namespace.
namespace GLib
{

float GetRandomFloat(float a, float b)
{
	if( a >= b ) // bad input
		return a;

	// Get random float in [0, 1] interval.
	float f = (rand()%10001) * 0.0001f;

	return (f*(b-a))+a;
}

wstring StringToWString(const std::string &s)
{
	return wstring(s.begin(), s.end());
}

unsigned int ColorRGBA(byte red, byte green, byte blue, byte alpha)
{
	unsigned int color = (alpha<<24) | (blue<<16) | (green<<8) | (red);

	return color;
}

vector<string> SplitString(string text, char delim)
{
	vector<string> elems;
	stringstream ss(text);
	string item;

	while(getline(ss, item, delim)) {
		elems.push_back(item);
	}

	return elems;
}

UINT32 StripRGBA(string colors)
{
	vector<string> split = SplitString(colors, ' ');
	return ColorRGBA(atoi(split[0].c_str()), atoi(split[1].c_str()), atoi(split[2].c_str()), atoi(split[3].c_str()));
}

bool IsNumber(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && (isdigit(*it) || *it == '.'))
		++it;

	return !s.empty() && it == s.end();
}

//! Loads and creates a SRV to a 2d texture array.
//! Taken from Frank Lunas book.
ID3D11ShaderResourceView* CreateTexture2DArraySRV(ID3D11Device* device, ID3D11DeviceContext* context, vector<string>& filenames)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//

	DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE;
	UINT filter = D3DX11_FILTER_NONE;
	UINT mipFilter = D3DX11_FILTER_LINEAR;

	UINT size = filenames.size();

	std::vector<ID3D11Texture2D*> srcTex(size);
	for(UINT i = 0; i < size; ++i)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;

        loadInfo.Width  = D3DX11_FROM_FILE;
        loadInfo.Height = D3DX11_FROM_FILE;
        loadInfo.Depth  = D3DX11_FROM_FILE;
        loadInfo.FirstMipLevel = 0;
        loadInfo.MipLevels = D3DX11_FROM_FILE;
        loadInfo.Usage = D3D11_USAGE_STAGING;
        loadInfo.BindFlags = 0;
        loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        loadInfo.MiscFlags = 0;
        loadInfo.Format = format;
        loadInfo.Filter = filter;
        loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo  = 0;

        HR(D3DX11CreateTextureFromFile(device, filenames[i].c_str(), 
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));
	}

	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = size;
	texArrayDesc.Format             = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	ID3D11Texture2D* texArray = 0;
	HR(device->CreateTexture2D( &texArrayDesc, 0, &texArray));

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for(UINT texElement = 0; texElement < size; ++texElement)
	{
		// for each mipmap level...
		for(UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			context->UpdateSubresource(texArray, 
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(srcTex[texElement], mipLevel);
		}
	}	

	//
	// Create a resource view to the texture array.
	//
	
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);

	for(UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return texArraySRV;
}

// Returns the inverse transpose.
XMMATRIX InverseTranspose(CXMMATRIX M)
{
	// Inverse-transpose is just applied to normals.  So zero out 
	// translation row so that it doesn't get into our inverse-transpose
	// calculation--we don't want the inverse-transpose of the translation.
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

//XMFLOAT4X4 ToXMFloat4X4(aiMatrix4x4 ai)
//{
//	XMFLOAT4X4 xm;
//
//	xm._11 = ai.a1;
//	xm._12 = ai.a2;
//	xm._13 = ai.a3;
//	xm._14 = ai.a4;
//
//	xm._21 = ai.b1;
//	xm._22 = ai.b2;
//	xm._23 = ai.b3;
//	xm._24 = ai.b4;
//
//	xm._31 = ai.c1;
//	xm._32 = ai.c2;
//	xm._33 = ai.c3;
//	xm._34 = ai.c4;
//
//	xm._41 = ai.d1;
//	xm._42 = ai.d2;
//	xm._43 = ai.d3;
//	xm._44 = ai.d4;
//
//	return xm;
//}

//-----------------------------------------------------------------------------
// Transform an axis aligned box by an angle preserving transform.
//-----------------------------------------------------------------------------
VOID TransformAxisAlignedBoxCustom(XNA::AxisAlignedBox* pOut, const XNA::AxisAlignedBox* pIn, FXMVECTOR Scale, FXMVECTOR Rotation, FXMVECTOR Translation )
{
    XMASSERT( pOut );
    XMASSERT( pIn );

    static XMVECTOR Offset[8] =
    {
        { -1.0f, -1.0f, -1.0f, 0.0f },
        { -1.0f, -1.0f,  1.0f, 0.0f },
        { -1.0f,  1.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f,  1.0f, 0.0f },
        {  1.0f, -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f,  1.0f, 0.0f },
        {  1.0f,  1.0f, -1.0f, 0.0f },
        {  1.0f,  1.0f,  1.0f, 0.0f }
    };

    // Load center and extents.
    XMVECTOR Center = XMLoadFloat3( &pIn->Center );
    XMVECTOR Extents = XMLoadFloat3( &pIn->Extents );

    XMVECTOR VectorScale = Scale;//XMVectorReplicate( Scale );

    // Compute and transform the corners and find new min/max bounds.
    XMVECTOR Corner = Center + Extents * Offset[0];
    Corner = XMVector3Rotate( Corner * VectorScale, Rotation ) + Translation;

    XMVECTOR Min, Max;
    Min = Max = Corner;

    for( INT i = 1; i < 8; i++ )
    {
        Corner = Center + Extents * Offset[i];
        Corner = XMVector3Rotate( Corner * VectorScale, Rotation ) + Translation;

        Min = XMVectorMin( Min, Corner );
        Max = XMVectorMax( Max, Corner );
    }

    // Store center and extents.
    XMStoreFloat3( &pOut->Center, ( Min + Max ) * 0.5f );
    XMStoreFloat3( &pOut->Extents, ( Max - Min ) * 0.5f );

    return;
}

//-----------------------------------------------------------------------------
// Compute the intersection of a ray (Origin, Direction) with a triangle 
// (V0, V1, V2).  Return TRUE if there is an intersection and also set *pDist 
// to the distance along the ray to the intersection.
// 
// The algorithm is based on Moller, Tomas and Trumbore, "Fast, Minimum Storage 
// Ray-Triangle Intersection", Journal of Graphics Tools, vol. 2, no. 1, 
// pp 21-28, 1997.
//-----------------------------------------------------------------------------
BOOL GLibIntersectRayTriangle( FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, CXMVECTOR V1, CXMVECTOR V2,
							  FLOAT* pDist )
{
	XMASSERT( pDist );
	//XMASSERT( XMVector3IsUnit( Direction ) );

	static const XMVECTOR Epsilon =
	{
		1e-20f, 1e-20f, 1e-20f, 1e-20f
	};

	XMVECTOR Zero = XMVectorZero();

	XMVECTOR e1 = V1 - V0;
	XMVECTOR e2 = V2 - V0;

	// p = Direction ^ e2;
	XMVECTOR p = XMVector3Cross( Direction, e2 );

	// det = e1 * p;
	XMVECTOR det = XMVector3Dot( e1, p );

	XMVECTOR u, v, t;

	if( XMVector3GreaterOrEqual( det, Epsilon ) )
	{
		// Determinate is positive (front side of the triangle).
		XMVECTOR s = Origin - V0;

		// u = s * p;
		u = XMVector3Dot( s, p );

		XMVECTOR NoIntersection = XMVectorLess( u, Zero );
		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( u, det ) );

		// q = s ^ e1;
		XMVECTOR q = XMVector3Cross( s, e1 );

		// v = Direction * q;
		v = XMVector3Dot( Direction, q );

		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( v, Zero ) );
		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( u + v, det ) );

		// t = e2 * q;
		t = XMVector3Dot( e2, q );

		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( t, Zero ) );

		if( XMVector4EqualInt( NoIntersection, XMVectorTrueInt() ) )
			return FALSE;
	}
	else if( XMVector3LessOrEqual( det, -Epsilon ) )
	{
		// Determinate is negative (back side of the triangle).
		XMVECTOR s = Origin - V0;

		// u = s * p;
		u = XMVector3Dot( s, p );

		XMVECTOR NoIntersection = XMVectorGreater( u, Zero );
		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( u, det ) );

		// q = s ^ e1;
		XMVECTOR q = XMVector3Cross( s, e1 );

		// v = Direction * q;
		v = XMVector3Dot( Direction, q );

		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( v, Zero ) );
		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( u + v, det ) );

		// t = e2 * q;
		t = XMVector3Dot( e2, q );

		NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( t, Zero ) );

		if ( XMVector4EqualInt( NoIntersection, XMVectorTrueInt() ) )
			return FALSE;
	}
	else
	{
		// Parallel ray.
		return FALSE;
	}

	XMVECTOR inv_det = XMVectorReciprocal( det );

	t *= inv_det;

	// u * inv_det and v * inv_det are the barycentric cooridinates of the intersection.

	// Store the x-component to *pDist
	XMStoreFloat( pDist, t );

	return TRUE;
}

//-----------------------------------------------------------------------------
// Return TRUE if any of the elements of a 3 vector are equal to 0xffffffff.
// Slightly more efficient than using XMVector3EqualInt.
//-----------------------------------------------------------------------------
static inline BOOL GLibXMVector3AnyTrue( FXMVECTOR V )
{
	XMVECTOR C;

	// Duplicate the fourth element from the first element.
	C = XMVectorSwizzle( V, 0, 1, 2, 0 );

	return XMComparisonAnyTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );
}

//-----------------------------------------------------------------------------
// Compute the intersection of a ray (Origin, Direction) with an axis aligned 
// box using the slabs method.
//-----------------------------------------------------------------------------
BOOL GLibIntersectRayAxisAlignedBox( FXMVECTOR Origin, FXMVECTOR Direction, const XNA::AxisAlignedBox* pVolume, FLOAT* pDist )
{
	XMASSERT( pVolume );
	XMASSERT( pDist );
	//XMASSERT( XMVector3IsUnit( Direction ) );

	static const XMVECTOR Epsilon =
	{
		1e-20f, 1e-20f, 1e-20f, 1e-20f
	};
	static const XMVECTOR FltMin =
	{
		-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX
	};
	static const XMVECTOR FltMax =
	{
		FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX
	};

	// Load the box.
	XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
	XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );

	// Adjust ray origin to be relative to center of the box.
	XMVECTOR TOrigin = Center - Origin;

	// Compute the dot product againt each axis of the box.
	// Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
	XMVECTOR AxisDotOrigin = TOrigin;
	XMVECTOR AxisDotDirection = Direction;

	// if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
	XMVECTOR IsParallel = XMVectorLessOrEqual( XMVectorAbs( AxisDotDirection ), Epsilon );

	// Test against all three axii simultaneously.
	XMVECTOR InverseAxisDotDirection = XMVectorReciprocal( AxisDotDirection );
	XMVECTOR t1 = ( AxisDotOrigin - Extents ) * InverseAxisDotDirection;
	XMVECTOR t2 = ( AxisDotOrigin + Extents ) * InverseAxisDotDirection;

	// Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
	// use the results from any directions parallel to the slab.
	XMVECTOR t_min = XMVectorSelect( XMVectorMin( t1, t2 ), FltMin, IsParallel );
	XMVECTOR t_max = XMVectorSelect( XMVectorMax( t1, t2 ), FltMax, IsParallel );

	// t_min.x = maximum( t_min.x, t_min.y, t_min.z );
	// t_max.x = minimum( t_max.x, t_max.y, t_max.z );
	t_min = XMVectorMax( t_min, XMVectorSplatY( t_min ) );  // x = max(x,y)
	t_min = XMVectorMax( t_min, XMVectorSplatZ( t_min ) );  // x = max(max(x,y),z)
	t_max = XMVectorMin( t_max, XMVectorSplatY( t_max ) );  // x = min(x,y)
	t_max = XMVectorMin( t_max, XMVectorSplatZ( t_max ) );  // x = min(min(x,y),z)

	// if ( t_min > t_max ) return FALSE;
	XMVECTOR NoIntersection = XMVectorGreater( XMVectorSplatX( t_min ), XMVectorSplatX( t_max ) );

	// if ( t_max < 0.0f ) return FALSE;
	NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( XMVectorSplatX( t_max ), XMVectorZero() ) );

	// if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return FALSE;
	XMVECTOR ParallelOverlap = XMVectorInBounds( AxisDotOrigin, Extents );
	NoIntersection = XMVectorOrInt( NoIntersection, XMVectorAndCInt( IsParallel, ParallelOverlap ) );

	if(!GLibXMVector3AnyTrue( NoIntersection ) )
	{
		// Store the x-component to *pDist
		XMStoreFloat( pDist, t_min );
		return TRUE;
	}

	return FALSE;
}

INT GLibIntersectAxisAlignedBoxFrustum( const XNA::AxisAlignedBox* pVolumeA, const XNA::Frustum* pVolumeB )
{
	return XNA::IntersectAxisAlignedBoxFrustum(pVolumeA, pVolumeB);
}

}	// End of Graphics Library namespace.


XMFLOAT3 operator+(const XMFLOAT3 a, const XMFLOAT3 b)
{
	XMVECTOR first = XMLoadFloat3(&a);
	XMVECTOR second = XMLoadFloat3(&b);

	XMFLOAT3 result;
	XMStoreFloat3(&result, first + second);
	return result;
}

XMFLOAT3 operator-(const XMFLOAT3 a, const XMFLOAT3 b)
{
	XMVECTOR first = XMLoadFloat3(&a);
	XMVECTOR second = XMLoadFloat3(&b);

	XMFLOAT3 result;
	XMStoreFloat3(&result, first - second);
	return result;
}

XMFLOAT3 operator+=(const XMFLOAT3 a, const XMVECTOR b)
{
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMLoadFloat3(&a) + b);
	return result;
}

XMFLOAT3 operator-=(const XMFLOAT3 a, const XMVECTOR b)
{
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMLoadFloat3(&a) - b);
	return result;
}

XMFLOAT4 operator*(const XMFLOAT4 a, const float b)
{
	return XMFLOAT4(a.x * b, a.y * b, a.z * b, a.w * b);
}

XMFLOAT3 operator*(const XMFLOAT3 a, const float b)
{
	return XMFLOAT3(a.x * b, a.y * b, a.z * b);
}

XMFLOAT3 operator*(const float b, const XMFLOAT3 a)
{
	return XMFLOAT3(a.x * b, a.y * b, a.z * b);
}


XMFLOAT2 operator+(const XMFLOAT2 a, const XMFLOAT2 b)
{
	XMVECTOR first = XMLoadFloat2(&a);
	XMVECTOR second = XMLoadFloat2(&b);

	XMFLOAT2 result;
	XMStoreFloat2(&result, first + second);
	return result;
}

XMFLOAT2 operator-(const XMFLOAT2 a, const XMFLOAT2 b)
{
	XMVECTOR first = XMLoadFloat2(&a);
	XMVECTOR second = XMLoadFloat2(&b);

	XMFLOAT2 result;
	XMStoreFloat2(&result, first - second);
	return result;
}

XMFLOAT3 operator+(const XMFLOAT3 a, const float b)
{
	return XMFLOAT3(a.x + b, a.y + b, a.z + b);
}
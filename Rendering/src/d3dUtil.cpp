#pragma warning( push )
#pragma warning (disable : 4005)

#include "d3dUtil.h"
#include <xnamath.h>
#include "Runnable.h"
#include "D3DCore.h"
#include "Graphics.h"
#include <string>
#include <vector>
#include <sstream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "Runnable.h"
using namespace std;

#pragma warning( pop ) 

//! Graphics Library namespace.
namespace GLib
{

GLIB_API Runnable* GlobalApp::mRunnable = nullptr;

void GlobalApp::SetRunnable(Runnable* pRunnable)
{
	mRunnable = pRunnable;
}

Runnable* GlobalApp::GetGame()
{
	return mRunnable;
}

ID3D11Device* GlobalApp::GetD3DDevice()
{
	return mRunnable->GetD3D()->GetDevice();
}

ID3D11DeviceContext* GlobalApp::GetD3DContext()
{
	return mRunnable->GetD3D()->GetContext();
}

Graphics* GlobalApp::GetGraphics()
{
	return mRunnable->GetGraphics();
}

Camera*	GlobalApp::GetCamera()
{
	return mRunnable->GetGraphics()->GetCamera();
}

HINSTANCE GlobalApp::GetAppInstance()
{
	return mRunnable->GetInstance();
}

HWND GlobalApp::GetWindowHandler()
{
	return mRunnable->GetHwnd();
}

int	GlobalApp::GetClientWidth()
{
	return mRunnable->GetClientWidth();
}

int GlobalApp::GetClientHeight()
{
	return mRunnable->GetClientHeight();
}

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
ID3D11ShaderResourceView* d3dHelper::CreateTexture2DArraySRV(ID3D11Device* device, ID3D11DeviceContext* context, vector<string>& filenames)
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
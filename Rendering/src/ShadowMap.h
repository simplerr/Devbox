#pragma once
#include "d3dUtil.h"

/**
	The steps to use shadow mapping:

	1.) Build the transform matrix from world space to texture space from the light perspective.
	2.) Render the scene to the shadow map from the light perspective, only depth writes.
	3.) Render the scene as normal but let ShadowPosH in the shader get transformed to texture space of the light.
	4.) Compare the z value of ShadowPosH with the z value stored in the shadow map at the same position.
	- If it's furhter away then the point representing ShadowPosH is in shadow.
	- If it's not further away then the it's not in shadow.
*/

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Light;

	//!
	//	Contains the shadow map texture with depth values.
	//	Contains matrices representing the light space, and the transform matrix to the texture space.
	//!
	class ShadowMap
	{
	public:
		ShadowMap(ID3D11Device* device, UINT width, UINT height);
		~ShadowMap();

		void BindDepthStencil(ID3D11DeviceContext* context);
		void BuildShadowTransform(Light* light);

		ID3D11ShaderResourceView* GetSRV();
		XMFLOAT4X4 GetLightView();
		XMFLOAT4X4 GetLightProj();
		XMFLOAT4X4 GetShadowTransform();
	private:
		ID3D11ShaderResourceView*	mDepthMapSRV;
		ID3D11DepthStencilView*		mDepthMapDSV;
		D3D11_VIEWPORT				mViewport;
		XMFLOAT4X4					mLightView;
		XMFLOAT4X4					mLightProj;
		XMFLOAT4X4					mShadowTransform;
		UINT						mWidth, mHeight;
		BoundingSphere				mSceneBounds;
	};
}
#include "ShadowMap.h"
#include "Light.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height)
	: mWidth(width), mHeight(height), mDepthMapDSV(0), mDepthMapSRV(0)
{
	// Setup the viewport.
	mViewport.TopLeftX = 0.0f;
    mViewport.TopLeftY = 0.0f;
    mViewport.Width    = static_cast<float>(width);
    mViewport.Height   = static_cast<float>(height);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

	// Fill out the texture desc.
	D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width     = mWidth;
    texDesc.Height    = mHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format    = DXGI_FORMAT_R24G8_TYPELESS;	// Typeless since the DSV and SRV will interpret it differently.
    texDesc.SampleDesc.Count   = 1;  
    texDesc.SampleDesc.Quality = 0;  
    texDesc.Usage          = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0; 
    texDesc.MiscFlags      = 0;

	// Create the Texture2D object that will be used both as a SRV and DSV.
	// It's used as a DSV to write depth values to it and SRV so we can use it as input to a shader.
	ID3D11Texture2D* depthMap = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &depthMap));

	// Create the SRV to the texture.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    HR(device->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV));

	// Create the DSV to the texture.
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV));

	// Don't need this anymore (stored in the views).
	ReleaseCOM(depthMap);

	// [NOTE]
	mSceneBounds.radius = 120.0f;
}
	
//! Cleanup.
ShadowMap::~ShadowMap()
{
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);
}

//! Binds the shadow map to receive depth writes.
void ShadowMap::BindDepthStencil(ID3D11DeviceContext* context)
{
	context->RSSetViewports(1, &mViewport);

	// Set NULL render target to avoid color writes.
	ID3D11RenderTargetView* renderTargets[1] = {0};
	context->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);
	context->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

//! Builds the matrix that transforms from world space to texture space from the light perspective.
void ShadowMap::BuildShadowTransform(Light* light)
{
	// Load the light information.
	XMVECTOR lightDir = XMLoadFloat3(&light->GetDirection());
	XMVECTOR lightPos = XMLoadFloat3(&light->GetPosition());
	XMVECTOR targetPos = lightPos + lightDir;//XMVectorSet(0, 0, 0, 0);	// [NOTE] Wrong!!!
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Create the light view matrix.
	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform the bounding sphere to light space, with the light view matrix.
	XMFLOAT3 sphereCenter;
	XMStoreFloat3(&sphereCenter, XMVector3TransformCoord(targetPos, V));

	// Create the orthogonal projection matrix so it covers the entire scene.
	float left = sphereCenter.x - mSceneBounds.radius;
	float bottom = sphereCenter.y - mSceneBounds.radius;
	float _near = sphereCenter.z - mSceneBounds.radius;
	float right = sphereCenter.x + mSceneBounds.radius;
	float top = sphereCenter.y + mSceneBounds.radius;
	float _far = sphereCenter.z + mSceneBounds.radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, _near, _far);

	// Transforms NDC space [-1,+1] to texture space [0,1].
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	// The shadow transform combines the view, projection and NDC-to-texture-space matrices.
	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

ID3D11ShaderResourceView* ShadowMap::GetSRV()
{
	return mDepthMapSRV;
}
	
XMFLOAT4X4 ShadowMap::GetLightView()
{
	return mLightView;
}
	
XMFLOAT4X4 ShadowMap::GetLightProj()
{
	return mLightProj;
}
	
XMFLOAT4X4 ShadowMap::GetShadowTransform()
{
	return mShadowTransform;
}

}	// End of Graphics Library namespace.
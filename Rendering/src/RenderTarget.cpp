#include "RenderTarget.h"
#include "Graphics.h"
#include "Utility.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
RenderTarget::RenderTarget(Graphics* pGraphics, UINT width, UINT height)
{
	// Fill out the D3D11_TEXTURE2D_DESC structure that will be used creating the ID3D11Texture2D texture.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Create the ID3D11Texture2D* texture.
	HR(pGraphics->GetDevice()->CreateTexture2D(&texDesc, 0, &mID3D11Texture2D));

	// Create the shader resource view and the render target view.
	mRenderTargetTexture = new Texture2D();
	HR(pGraphics->GetDevice()->CreateShaderResourceView(mID3D11Texture2D, 0, &mRenderTargetTexture->shaderResourceView));
	HR(pGraphics->GetDevice()->CreateRenderTargetView(mID3D11Texture2D, 0, &mRenderTargetView));

	// Change the format and bind flags for the D3D11_TEXTURE2D_DESC variable created above.
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Create the ID3D11Texture2D to use as stencil/back buffer.
	HR(pGraphics->GetDevice()->CreateTexture2D(&texDesc, 0, &mDepthStencilBuffer));
	HR(pGraphics->GetDevice()->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));

	mWidth	= width;
	mHeight	= height;
}

//! Cleanup.
RenderTarget::~RenderTarget()
{

}

//! Returns the render target view.
ID3D11RenderTargetView* RenderTarget::GetRenderTargetView()
{
	return mRenderTargetView;
}
	
//! Returns the render target texture.
Texture2D* RenderTarget::GetRenderTargetTexture()
{
	return mRenderTargetTexture;
}

//! Returns the shader resource view.
ID3D11ShaderResourceView* RenderTarget::GetShaderResourceView()
{
	return mRenderTargetTexture->shaderResourceView;
}

//! Returns the depth stencil view.
ID3D11DepthStencilView*	RenderTarget::GetDepthStencilView()
{
	return mDepthStencilView;
}
	
//! Returns the ID3D11Texture2D that all views was created from.
ID3D11Texture2D* RenderTarget::GetID3D11Texture2D()
{
	return mID3D11Texture2D;
}

}	// End of Graphics Library namespace.
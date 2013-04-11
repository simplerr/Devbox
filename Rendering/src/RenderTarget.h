#pragma once
#include <D3D11.h>
#include <string>
using namespace std;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Graphics;
	struct Texture2D;

	//! A render target wrapper.
	class RenderTarget
	{
	public:
		RenderTarget(Graphics* pGraphics, UINT width, UINT height);
		~RenderTarget();

		ID3D11RenderTargetView*		GetRenderTargetView();
		ID3D11ShaderResourceView*	GetShaderResourceView();
		ID3D11DepthStencilView*		GetDepthStencilView();
		ID3D11Texture2D*			GetID3D11Texture2D();
		Texture2D*					GetRenderTargetTexture();
	private:
		ID3D11RenderTargetView*		mRenderTargetView;
		Texture2D*					mRenderTargetTexture;	// Contains ID3D11ShaderResourceView*
		ID3D11DepthStencilView*		mDepthStencilView;
		ID3D11Texture2D*			mDepthStencilBuffer;
		ID3D11Texture2D*			mID3D11Texture2D;
		UINT mWidth;
		UINT mHeight;
	};
}
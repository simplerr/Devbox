#pragma once

#include <D3DX11.h>

//! Graphics Library namespace.
namespace GLib
{
	//!
	//	Applies a blur filter to a texture.
	//!
	class BlurFilter
	{
	public:
		BlurFilter();
		~BlurFilter();

		void Init(ID3D11Device* device, UINT width, UINT height);
		void ApplyBlur(ID3D11Device* pDevice, ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputSRV, int blurCount);
	private:
		ID3D11ShaderResourceView*	mInputSRV;
		ID3D11ShaderResourceView*	mTmpSRV;
		ID3D11UnorderedAccessView*	mInputUAV;
		ID3D11UnorderedAccessView*	mTmpUAV;
		UINT mWidth, mHeight;
	};
}
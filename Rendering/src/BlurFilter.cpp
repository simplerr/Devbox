#include "BlurFilter.h"
#include "d3dUtil.h"
#include "Runnable.h"
#include "Graphics.h"
#include "Effects.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
BlurFilter::BlurFilter()
{
	mTmpSRV = 0;
	mTmpUAV = 0;
	mInputUAV = 0;
}
	
//! Cleanup.
BlurFilter::~BlurFilter()
{
	ReleaseCOM(mTmpSRV);
	ReleaseCOM(mTmpUAV);
	ReleaseCOM(mInputUAV);
}

//! Inits the SRVs and UAVs.
void BlurFilter::Init(ID3D11Device* device, UINT width, UINT height)
{
	// Start clean.
	ReleaseCOM(mTmpSRV);
	ReleaseCOM(mTmpUAV);

	mWidth = width;
	mHeight = height;

	// Fill out the D3D11_TEXTURE2D_DESC.
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_TEXTURE2D_DESC blurredTexDesc;
	blurredTexDesc.Width     = width;
	blurredTexDesc.Height    = height;
    blurredTexDesc.MipLevels = 1;
    blurredTexDesc.ArraySize = 1;
	blurredTexDesc.Format    = format;
	blurredTexDesc.SampleDesc.Count   = 1;
	blurredTexDesc.SampleDesc.Quality = 0;
    blurredTexDesc.Usage     = D3D11_USAGE_DEFAULT;
    blurredTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    blurredTexDesc.CPUAccessFlags = 0;
    blurredTexDesc.MiscFlags      = 0;

	// Create the ID3D11Texture2D*.
	ID3D11Texture2D* blurredTex = 0;
	HR(device->CreateTexture2D(&blurredTexDesc, 0, &blurredTex));

	// Create the SRV and UAV. 
	// [NOTE] passes NULL as the desc, not like in the book!
	HR(device->CreateShaderResourceView(blurredTex, 0, &mTmpSRV));
	HR(device->CreateUnorderedAccessView(blurredTex, 0, &mTmpUAV));

	// Views save a reference to the texture so we can release our reference.
	ReleaseCOM(blurredTex);
}
	
//! Applies the blur effect.
void BlurFilter::ApplyBlur(ID3D11Device* pDevice, ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputSRV, int blurCount)
{
	// Get the ID3D11Texture2D and it's description.
	ID3D11Resource* resource = 0;
	inputSRV->GetResource(&resource);
	ID3D11Texture2D* texture = (ID3D11Texture2D*)resource;
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	// Is the width & height correct?
	if(desc.Width != mWidth || desc.Height != mHeight)
		Init(pDevice, desc.Width, desc.Height);

	// Create the UAV for the input SRV.
	HR(pDevice->CreateUnorderedAccessView(texture, 0, &mInputUAV));

	mInputSRV = inputSRV;

	for(int i = 0; i < blurCount; i++)
	{
		// Horizontal pass.
		Effects::BlurFX->SetInputMap(inputSRV);
		Effects::BlurFX->SetOutputMap(mTmpUAV);
		Effects::BlurFX->HorzTech->GetPassByIndex(0)->Apply(0, context);

		UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
		context->Dispatch(numGroupsX, mHeight, 1);

		// Unbind the input texture from the CS.
		ID3D11ShaderResourceView* nullSRV[1] = { 0 };
		context->CSSetShaderResources(0, 1, nullSRV);

		// Unbind the output texture from the CS.
		ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
		context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

		// Vertical pass.
		Effects::BlurFX->SetInputMap(mTmpSRV);
		Effects::BlurFX->SetOutputMap(mInputUAV);
		Effects::BlurFX->VertTech->GetPassByIndex(0)->Apply(0, context);

		UINT numGroupsY = (UINT)ceilf(mHeight / 256.0f);
		context->Dispatch(mWidth, numGroupsY, 1);

		context->CSSetShaderResources(0, 1, nullSRV);
		context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	// Disable compute shader.
	context->CSSetShader(0, 0, 0);
}

}	// End of Graphics Library namespace.
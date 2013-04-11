#include "RenderStates.h"
#include "d3dUtil.h"

//! Graphics Library namespace.
namespace GLib
{

ID3D11BlendState*			RenderStates::TransparentBS     = 0;
ID3D11DepthStencilState*	RenderStates::EnableAllDSS		= 0;  
ID3D11DepthStencilState*	RenderStates::NoDoubleBlendDSS  = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	//
	// The transparent blend state.
	//
	D3D11_BLEND_DESC transparentDesc = {0};
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable	   = true;
	transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));

	//
	// The enable all depth stencil state.
	//
	D3D11_DEPTH_STENCIL_DESC depthdesc;
	memset(&depthdesc, 0, sizeof(depthdesc));
	depthdesc.DepthEnable = (BOOL) false;
	depthdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthdesc.DepthFunc = D3D11_COMPARISON_NEVER;
	depthdesc.StencilEnable = (BOOL) false;
	HR(device->CreateDepthStencilState(&depthdesc, &EnableAllDSS));

	//
	// The no double blending stencil state.
	//
	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable      = true;
	noDoubleBlendDesc.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.DepthFunc        = D3D11_COMPARISON_LESS; 
	noDoubleBlendDesc.StencilEnable    = true;
	noDoubleBlendDesc.StencilReadMask  = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;

	noDoubleBlendDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilFunc   = D3D11_COMPARISON_EQUAL;

	noDoubleBlendDesc.BackFace = noDoubleBlendDesc.FrontFace;

	// Don't render the backfacing triangles, the key of this render state..
	noDoubleBlendDesc.BackFace.StencilFunc   = D3D11_COMPARISON_NEVER;
}

//! Cleanup.
void RenderStates::DestroyAll()
{
	ReleaseCOM(TransparentBS);
	ReleaseCOM(EnableAllDSS);
	ReleaseCOM(NoDoubleBlendDSS);
}

}	// End of Graphics Library namespace.
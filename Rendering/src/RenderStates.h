#pragma once

#include <D3DX11.h>

//! Graphics Library namespace.
namespace GLib
{
	//! Contains the different render states.
	class RenderStates
	{
	public:
		static void InitAll(ID3D11Device* device);
		static void DestroyAll();

		static ID3D11BlendState*		TransparentBS;
		static ID3D11DepthStencilState* EnableAllDSS;
		static ID3D11DepthStencilState* NoDoubleBlendDSS;
	};
}
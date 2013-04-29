#pragma once

#include "Effects.h"

class GLIB_API VoxelEffect : public GLib::Effect
{
public:
	VoxelEffect::VoxelEffect(ID3D11Device* pDevice)
		: Effect(pDevice, "Voxel.fx", "VoxelTech")
	{

	}

	VoxelEffect::~VoxelEffect()
	{

	}

	void Init()
	{
		mfxWVP		= mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		mfxWorld	= mEffect->GetVariableByName("gWorld")->AsMatrix();
		mfxWorldInvTranspose	= mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();	

		mfxColor	= mEffect->GetVariableByName("gColor")->AsVector();
	}

	void CreateInputLayout(ID3D11Device* pDevice)
	{
		// Create the vertex input layout.
		D3D11_INPUT_ELEMENT_DESC vertexDesc[2] =
		{
			{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		// Create the input layout.
		D3DX11_PASS_DESC passDesc;
		mTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(pDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
			passDesc.IAInputSignatureSize, &mInputLayout));
	}

	void SetWorldViewProj(CXMMATRIX matrix)			{ mfxWVP->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetWorld(CXMMATRIX matrix)					{ mfxWorld->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetWorldInvTranspose(CXMMATRIX matrix)		{ mfxWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&matrix)); }

	void SetColor(XMFLOAT4 color)					{ mfxColor->SetFloatVector((const float*)&color); }
private:
	ID3DX11EffectMatrixVariable* mfxWVP;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;

	ID3DX11EffectVectorVariable* mfxColor;
};
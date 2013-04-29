#pragma once

#include "Effects.h"

class GLIB_API BoundingBoxEffect : public GLib::Effect
{
public:
	BoundingBoxEffect::BoundingBoxEffect(ID3D11Device* pDevice)
		: Effect(pDevice, "BoundingBox.fx", "SolidTech")
	{

	}

	BoundingBoxEffect::~BoundingBoxEffect()
	{

	}

	void Init()
	{
		mfxWVP		= mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		mfxColor	= mEffect->GetVariableByName("gColor")->AsVector();

		mfxWireframeTech		 = mEffect->GetTechniqueByName("WireframeTech");
	}

	void CreateInputLayout(ID3D11Device* pDevice)
	{
		// Create the vertex input layout.
		D3D11_INPUT_ELEMENT_DESC vertexDesc[1] =
		{
			{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		// Create the input layout.
		D3DX11_PASS_DESC passDesc;
		mTech->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(pDevice->CreateInputLayout(vertexDesc, 1, passDesc.pIAInputSignature, 
			passDesc.IAInputSignatureSize, &mInputLayout));
	}

	void SetWorldViewProj(CXMMATRIX matrix)			{ mfxWVP->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetColor(XMFLOAT4 color)					{ mfxColor->SetFloatVector((const float*)&color); }

	void Apply(ID3D11DeviceContext* pContext, bool wireframe)
	{
		if(wireframe)
			mfxWireframeTech->GetPassByIndex(0)->Apply(0, pContext);
		else
			Effect::Apply(pContext);
	}

private:
	ID3DX11EffectMatrixVariable* mfxWVP;
	ID3DX11EffectVectorVariable* mfxColor;

	ID3DX11EffectTechnique*		 mfxWireframeTech;
};
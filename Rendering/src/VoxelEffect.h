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
		// Connect handlers to the effect variables.
		mfxWVP				 = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		mfxWorld			 = mEffect->GetVariableByName("gWorld")->AsMatrix();
		mfxWorldInvTranspose = mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
		mfxEyePosW			 = mEffect->GetVariableByName("gEyePosW")->AsVector();
		mfxLights			 = mEffect->GetVariableByName("gLights");
		mfxMaterial			 = mEffect->GetVariableByName("gMaterial");
		mfxNumLights		 = mEffect->GetVariableByName("gNumLights");
		mfxFogStart			 = mEffect->GetVariableByName("gFogStart")->AsScalar();
		mfxFogRange			 = mEffect->GetVariableByName("gFogRange")->AsScalar();
		mfxFogColor			 = mEffect->GetVariableByName("gFogColor")->AsVector();
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

	void SetEyePosition(XMFLOAT3 eyePos)			{ mfxEyePosW->SetRawValue(&eyePos, 0, sizeof(XMFLOAT3)); }
	void SetMaterial(GLib::Material material)		{ mfxMaterial->SetRawValue(&material, 0, sizeof(material)); }
	void SetFogRange(float range)					{ mfxFogRange->SetFloat(range); }
	void SetFogStart(float start)					{ mfxFogStart->SetFloat(start); }
	void SetFogColor(XMFLOAT4 color)				{ mfxFogColor->SetFloatVector((const float*)&color); }

	void SetLights(GLib::LightList* lights)	
	{
		if(lights != nullptr) {
			GLib::Light lightArray[10];
			for(unsigned int i = 0; i < lights->size() && i < 10; i++)
				lightArray[i] = *lights->operator[](i);

			// Sets the light list in the effect file.
			float size = min(10, lights->size());
			mfxLights->SetRawValue((void*)lightArray, 0, sizeof(GLib::Light) * size);
			mfxNumLights->SetRawValue(&size, 0, sizeof(float));
		}
		else {
			float size = 0;
			mfxNumLights->SetRawValue(&size, 0, sizeof(float));
		}
	}
private:
	ID3DX11EffectMatrixVariable* mfxWVP;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;

	ID3DX11EffectVectorVariable* mfxColor;

	ID3DX11EffectVectorVariable* mfxEyePosW;
	ID3DX11EffectVariable* mfxLights;
	ID3DX11EffectVariable* mfxMaterial;
	ID3DX11EffectVariable* mfxNumLights;

	// Fog.
	ID3DX11EffectScalarVariable* mfxFogStart;
	ID3DX11EffectScalarVariable* mfxFogRange;
	ID3DX11EffectVectorVariable* mfxFogColor;
};
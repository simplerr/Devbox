#include <fstream>
#include "Effects.h"
#include "Runnable.h"
#include "D3DCore.h"
#include "Utility.h"
#include "Light.h"
#include "Graphics.h"

//! Graphics Library namespace.
namespace GLib
{

//! Initialize the effects.
BasicEffect*		Effects::BasicFX		= nullptr;
BillboardEffect*	Effects::BillboardFX	= nullptr;
BlurEffect*			Effects::BlurFX			= nullptr;
SkyEffect*			Effects::SkyFX			= nullptr;
ShadowMapEffect*	Effects::BuildShadowMapFX = nullptr;
TerrainEffect*		Effects::TerrainFX		= nullptr;
ScreenEffect*		Effects::ScreenFX		= nullptr;
ParticleEffect*		Effects::ParticleFX		= nullptr;

#pragma region Code for the static effect handler Effects.

//! Inits all effects.
void Effects::InitAll(ID3D11Device* pDevice)
{
	/**
		Init the basic effect.
	*/
	BasicFX = new BasicEffect(pDevice);
	BasicFX->CreateInputLayout(pDevice);
	BasicFX->Init();

	/**
		Init the billboard effect.
	*/
	BillboardFX = new BillboardEffect(pDevice);
	BillboardFX->CreateInputLayout(pDevice);
	BillboardFX->Init();

	/**
		Init the blur effect.
	*/
	BlurFX = new BlurEffect(pDevice);
	BlurFX->Init();

	/**
		Init the sky box effect.
	*/
	SkyFX = new SkyEffect(pDevice);
	SkyFX->CreateInputLayout(pDevice);
	SkyFX->Init();

	// Init the build shadow map effect.
	BuildShadowMapFX = new ShadowMapEffect(pDevice);
	BuildShadowMapFX->CreateInputLayout(pDevice);
	BuildShadowMapFX->Init();

	// Init the terrrain effect.
	TerrainFX = new TerrainEffect(pDevice);
	TerrainFX->CreateInputLayout(pDevice);
	TerrainFX->Init();

	ScreenFX = new ScreenEffect(pDevice);
	ScreenFX->CreateInputLayout(pDevice);
	ScreenFX->Init();
}

//! Cleans up all effects.
void Effects::DestroyAll()
{
	delete BasicFX;
	delete BillboardFX;
	delete BlurFX;
	delete SkyFX;
	delete BuildShadowMapFX;
	delete TerrainFX;
	delete ScreenFX;
}

BasicEffect* Effects::GetBasicFX()
{
	return BasicFX;
}

#pragma endregion

#pragma region Code for the effect base class Effect.

//! Loads the effect from the file and creates the standard technique.
Effect::Effect(ID3D11Device* pDevice, string filename, string technique)
{
	// Create the effect from memory.
	// [NOTE] Fix this!
	//std::ifstream fin("fx/" + filename + "o", std::ios::binary);	// [NOTE]!!!
	//std::ifstream fin("F:\\Users\\Axel\\Documents\\Visual Studio 11\\Projects\\Graphics Library\\Graphics Library\\Common\\" + filename + "o", std::ios::binary);	// [NOTE]!!!
	std::ifstream fin("F:\\Users\\Axel\\Documents\\Projects\\Devbox\\Rendering\\src\\fx\\" + filename + "o", std::ios::binary);	// [NOTE]!!!

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, pDevice, &mEffect));

	// Set the technique.
	mTech = mEffect->GetTechniqueByName(technique.c_str());
}
	
Effect::~Effect()
{
	ReleaseCOM(mEffect);
}

//! Applies the effect to the render pipeline.
void Effect::Apply(ID3D11DeviceContext* pContext, EffectTech tech)
{
	// Applies the changes effect and it changes to the pipeline.
	mTech->GetPassByIndex(0)->Apply(0, pContext);
}

//! Returns the ID3DX11Effect member.
ID3DX11Effect* Effect::GetFx()
{
	return mEffect;
}

//! Returns the standard technique.
ID3DX11EffectTechnique* Effect::GetTech()
{
	return mTech;
}
	
//! Returns the input layout.
ID3D11InputLayout* Effect::GetInputLayout()
{
	return mInputLayout;
}

//! Sets the ID3DX11Effect member.
void Effect::SetEffect(ID3DX11Effect* effect)
{
	mEffect = effect;
}
	
//! Sets the standard technique.
void Effect::SetTech(string technique)
{
	mTech = mEffect->GetTechniqueByName(technique.c_str());
}

#pragma endregion

#pragma region Code for the basic effect class BasicEffect.

BasicEffect::BasicEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "Basic.fx", "BasicTech")
{

}
	
BasicEffect::~BasicEffect()
{

}

void BasicEffect::SetUseNormalMap(bool use)
{
	HRESULT hr = mfxUseNormalMap->SetBool(use);
}

//! Inits all effect variable handles.
void BasicEffect::Init()
{
	// Connect handlers to the effect variables.
	mfxSkinnedTech		 = mEffect->GetTechniqueByName("SkinnedTech");
	mfxWVP				 = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxWorld			 = mEffect->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose = mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxTexTransform		 = mEffect->GetVariableByName("gTexTransform")->AsMatrix();
	mfxBoneTransforms	 = mEffect->GetVariableByName("gBoneTransforms")->AsMatrix();
	mfxShadowTransform   = mEffect->GetVariableByName("gShadowTransform")->AsMatrix();
	mfxEyePosW			 = mEffect->GetVariableByName("gEyePosW")->AsVector();
	mfxLights			 = mEffect->GetVariableByName("gLights");
	mfxMaterial			 = mEffect->GetVariableByName("gMaterial");
	mfxNumLights		 = mEffect->GetVariableByName("gNumLights");
	mfxUseTexture		 = mEffect->GetVariableByName("gUseTexture")->AsScalar();
	mfxTexture			 = mEffect->GetVariableByName("gTexture")->AsShaderResource();
	mfxFogStart			 = mEffect->GetVariableByName("gFogStart")->AsScalar();
	mfxFogRange			 = mEffect->GetVariableByName("gFogRange")->AsScalar();
	mfxFogColor			 = mEffect->GetVariableByName("gFogColor")->AsVector();
	mfxNormalMap		 = mEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	mfxShadowMap         = mEffect->GetVariableByName("gShadowMap")->AsShaderResource();
	mfxUseNormalMap		 = mEffect->GetVariableByName("gUseNormalMap")->AsScalar();
	mfxUseAnimation		 = mEffect->GetVariableByName("gUseAnimation")->AsScalar();
	mfxUseLighting		 = mEffect->GetVariableByName("gUseLighting")->AsScalar();	
	mfxRenderingToShadowMap = mEffect->GetVariableByName("gRenderingToShadowMap")->AsScalar();
	mfxNormalMapTech	 = mEffect->GetTechniqueByName("NormalMapTech");
	mfxShadowMapTech	 = mEffect->GetTechniqueByName("ShadowMapTech");
}

//! Creates the input layout that will get set before the Input-Assembler state. The EffectManager calls this function.
void BasicEffect::CreateInputLayout(ID3D11Device* pDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[6] =
	{
		{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 6, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void BasicEffect::Apply(ID3D11DeviceContext* pContext, EffectTech tech)
{
	// Applies the changes effect and it changes to the pipeline.
	if(tech == STANDARD_TECH)
		Effect::Apply(pContext, tech);
	else if(tech == NMAP_TECH)
		mfxNormalMapTech->GetPassByIndex(0)->Apply(0, pContext);
	else if(tech = SMAP_TECH)
		mfxShadowMapTech->GetPassByIndex(0)->Apply(0, pContext);
}

//! Sets the lights to use in the shader.
void BasicEffect::SetLights(LightList* lights)
{
	if(lights != nullptr) {
		Light lightArray[10];
		for(unsigned int i = 0; i < lights->size() && i < 10; i++)
			lightArray[i] = *lights->operator[](i);

		// Sets the light list in the effect file.
		float size = min(10, lights->size());
		mfxLights->SetRawValue((void*)lightArray, 0, sizeof(Light) * size);
		mfxNumLights->SetRawValue(&size, 0, sizeof(float));
	}
	else {
		float size = 0;
		mfxNumLights->SetRawValue(&size, 0, sizeof(float));
	}
}

//! Sets the texture to use in the shader.
void BasicEffect::SetTexture(Texture2D* texture)
{
	bool f = texture == nullptr ? false : true;
	SetUseTexture(texture == nullptr ? false : true);
	if(texture != nullptr) {
		mfxTexture->SetResource(texture->shaderResourceView);
		XMMATRIX transform = XMMatrixScaling(texture->scale, texture->scale, 0);
		mfxTexTransform->SetMatrix((const float*)&transform);
	}
}

//! Sets the normal map to use.
void BasicEffect::SetNormalMap(Texture2D* normalMap)
{
	SetUseNormalMap(normalMap == nullptr ? false : true);
	if(normalMap != nullptr) {
		mfxNormalMap->SetResource(normalMap->shaderResourceView);
	}
}

ID3DX11EffectTechnique* BasicEffect::GetSkinnedTech()
{
	return mfxSkinnedTech;
}

#pragma endregion

#pragma region Code for the billboard effect class BillboardEffect.

BillboardEffect::BillboardEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "Billboard.fx", "BillboardTech")
{

}
	
BillboardEffect::~BillboardEffect()
{

}

void BillboardEffect::Init()
{
	// Connect handles to the effect variables.
	mfxViewProj		= mEffect->GetVariableByName("gViewProj")->AsMatrix();
	mfxEyePosW		= mEffect->GetVariableByName("gEyePosW")->AsVector();
	mfxFogColor		= mEffect->GetVariableByName("gFogColor")->AsVector();
	mfxFogStart		= mEffect->GetVariableByName("gFogStart")->AsScalar();
	mfxFogRange		= mEffect->GetVariableByName("gFogRange")->AsScalar();
	mfxLights		= mEffect->GetVariableByName("gLights");
	mfxMaterial		= mEffect->GetVariableByName("gMaterial");
	mfxTexture		= mEffect->GetVariableByName("gTexture")->AsShaderResource();
	mfxUseTexture	= mEffect->GetVariableByName("gUseTexture");
	mfxNumLights	= mEffect->GetVariableByName("gNumLights");
}
	
void BillboardEffect::CreateInputLayout(ID3D11Device* pDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",   0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void BillboardEffect::SetTexture(Texture2D* texture)
{
	SetUseTexture(texture == nullptr ? false : true);
	mfxTexture->SetResource(texture->shaderResourceView);
}
	
void BillboardEffect::SetLights(LightList* lights)
{
	if(lights != nullptr) {
		Light lightArray[10];
		for(int i = 0; i < lights->size() && i < 10; i++)
			lightArray[i] = *lights->operator[](i);

		// Sets the light list in the effect file.
		float size = min(10, lights->size());
		mfxLights->SetRawValue((void*)lightArray, 0, sizeof(Light) * size);
		mfxNumLights->SetRawValue(&size, 0, sizeof(float));
	}
	else {
		float size = 0;
		mfxNumLights->SetRawValue(&size, 0, sizeof(float));
	}
}
#pragma endregion

#pragma region BlurEffect
BlurEffect::BlurEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "Blur.fx", "VertBlur")
{

}
	
BlurEffect::~BlurEffect()
{

}

void BlurEffect::Init()
{
	HorzTech	= mEffect->GetTechniqueByName("HorzBlur");
	VertTech	= mEffect->GetTechniqueByName("VertBlur");

	Weights     = mEffect->GetVariableByName("gWeights")->AsScalar();
	InputMap    = mEffect->GetVariableByName("gInput")->AsShaderResource();
	OutputMap   = mEffect->GetVariableByName("gOutput")->AsUnorderedAccessView();
}
#pragma endregion

#pragma region Code for the sky box effect SkyEffect.

SkyEffect::SkyEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "Sky.fx", "SkyTech")
{
	
}

SkyEffect::~SkyEffect()
{

}

void SkyEffect::Init()
{
	WorldViewProj = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap       = mEffect->GetVariableByName("gCubeMap")->AsShaderResource();
}

//! Creates the input layout that will get set before the Input-Assembler state. The EffectManager calls this function.
void SkyEffect::CreateInputLayout(ID3D11Device* pDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}

#pragma endregion

#pragma region ShadowMapEffect

ShadowMapEffect::ShadowMapEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "BuildShadowMap.fx", "BuildShadowMapTech")
{

}
	
ShadowMapEffect::~ShadowMapEffect()
{

}

void ShadowMapEffect::Init()
{
	ViewProj          = mEffect->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj     = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mEffect->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
}

void ShadowMapEffect::CreateInputLayout(ID3D11Device* pDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 1, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}

#pragma endregion

#pragma region TerrainEffect

TerrainEffect::TerrainEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "Terrain.fx", "TerrainTech")
{
	
}
	
TerrainEffect::~TerrainEffect()
{

}

void TerrainEffect::Init()
{
	ViewProj           = mEffect->GetVariableByName("gViewProj")->AsMatrix();
	TexScale		   = mEffect->GetVariableByName("gTexScale")->AsScalar();
	EyePosW            = mEffect->GetVariableByName("gEyePosW")->AsVector();
	FogColor           = mEffect->GetVariableByName("gFogColor")->AsVector();
	FogStart           = mEffect->GetVariableByName("gFogStart")->AsScalar();
	FogRange           = mEffect->GetVariableByName("gFogRange")->AsScalar();
	Lights			   = mEffect->GetVariableByName("gLights");
	NumLights		   = mEffect->GetVariableByName("gNumLights");
	Mat                = mEffect->GetVariableByName("gMaterial");
	ShadowMap          = mEffect->GetVariableByName("gShadowMap")->AsShaderResource();
	ShadowTransform    = mEffect->GetVariableByName("gShadowTransform")->AsMatrix();
	TexelCellSpaceU    = mEffect->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	TexelCellSpaceV    = mEffect->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	WorldCellSpace     = mEffect->GetVariableByName("gWorldCellSpace")->AsScalar();

	LayerMapArray      = mEffect->GetVariableByName("gLayerMapArray")->AsShaderResource();
	BlendMap           = mEffect->GetVariableByName("gBlendMap")->AsShaderResource();
	HeightMap          = mEffect->GetVariableByName("gHeightMap")->AsShaderResource();

	ToolCenter			= mEffect->GetVariableByName("gToolCenter")->AsVector();
	ToolRadius          = mEffect->GetVariableByName("gToolRadius")->AsScalar();
	ArenaRadius         = mEffect->GetVariableByName("gArenaRadius")->AsScalar();
}
	
void TerrainEffect::CreateInputLayout(ID3D11Device* pDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 4, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}

//! Sets the lights to use in the shader.
void TerrainEffect::SetLights(LightList* lights)
{
	if(lights != nullptr) {
		Light lightArray[10];
		for(int i = 0; i < lights->size() && i < 10; i++)
			lightArray[i] = *lights->operator[](i);

		// Sets the light list in the effect file.
		float size = min(10, lights->size());
		Lights->SetRawValue((void*)lightArray, 0, sizeof(Light) * size);
		NumLights->SetRawValue(&size, 0, sizeof(float));
	}
	else {
		float size = 0;
		NumLights->SetRawValue(&size, 0, sizeof(float));
	}
}

#pragma endregion

#pragma region ScreenEffect
ScreenEffect::ScreenEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "ScreenQuad.fx", "BasicTech")
{

}

ScreenEffect::~ScreenEffect()
{

}
//! Inits all effect variable handles.
void ScreenEffect::Init()
{
	// Connect handlers to the effect variables.
	mfxWVP				 = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxWorld			 = mEffect->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose = mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxEyePosW			 = mEffect->GetVariableByName("gEyePosW")->AsVector();
	mfxTexture			 = mEffect->GetVariableByName("gTexture")->AsShaderResource();
}

//! Creates the input layout that will get set before the Input-Assembler state. The EffectManager calls this function.
void ScreenEffect::CreateInputLayout(ID3D11Device* pDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[6] =
	{
		{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout.
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 6, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void ScreenEffect::Apply(ID3D11DeviceContext* pContext, EffectTech tech)
{
	Effect::Apply(pContext, tech);
}

//! Sets the texture to use in the shader.
void ScreenEffect::SetTexture(Texture2D* texture)
{
	if(texture != nullptr) {
		mfxTexture->SetResource(texture->shaderResourceView);
	}
}

#pragma endregion

ParticleEffect::ParticleEffect(ID3D11Device* pDevice)
	: Effect(pDevice, "ParticleSystem.fx", "BasicTech")
{

}

ParticleEffect::~ParticleEffect()
{

}
//! Inits all effect variable handles.
void ParticleEffect::Init()
{
	// Connect handlers to the effect variables.
	mfxWVP				= mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxEyePosW			= mEffect->GetVariableByName("gEyePosW")->AsVector();
	mfxAccel			= mEffect->GetVariableByName("gAccel")->AsVector();
	mfxTexture			= mEffect->GetVariableByName("gTexture")->AsShaderResource();
	mfxTime				= mEffect->GetVariableByName("gTime")->AsScalar();
	mfxViewportHeight	= mEffect->GetVariableByName("gViewportHeight")->AsScalar();
}

//! Creates the input layout that will get set before the Input-Assembler state. The EffectManager calls this function.
void ParticleEffect::CreateInputLayout(ID3D11Device* pDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[7] =
	{
		{"INIT_POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"INIT_VELOCITY",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"INIT_SIZE",		0, DXGI_FORMAT_R32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"INIT_TIME",		0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"LIFE_TIME",		0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"MASS",			0, DXGI_FORMAT_R32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"INIT_COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,  D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout.
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pDevice->CreateInputLayout(vertexDesc, 7, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void ParticleEffect::Apply(ID3D11DeviceContext* pContext, EffectTech tech)
{
	Effect::Apply(pContext, tech);
}

//! Sets the texture to use in the shader.
void ParticleEffect::SetTexture(Texture2D* texture)
{
	if(texture != nullptr) {
		mfxTexture->SetResource(texture->shaderResourceView);
	}
}

}	// End of Graphics Library namespace.
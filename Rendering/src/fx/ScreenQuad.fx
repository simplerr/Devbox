#include "LightHelper.fx"
 
//! Constant buffers that updates per frame.
cbuffer cbPerFrame
{
	// Lights & num lights.
	Light	gLights[10];
	float	gNumLights;

	// Eye position (world space).
	float3	gEyePosW;

	// Fog.
	float	gFogStart;
	float	gFogRange;
	float4	gFogColor;
};

//! Constant buffers that updates per object.
cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	float4x4 gShadowTransform; 
	Material gMaterial;
	bool	 gUseTexture;
	bool	 gUseNormalMap;
	bool	 gUseAnimation;
	bool	 gRenderingToShadowMap;
	bool	 gUseLighting;
};


//! The texture and normal map to use.
Texture2D gTexture;

//! The sampler state to use with the texture.
SamplerState textureSampler
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

//! Input parameter to the vertex shader.
struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 Tex		: TEXCOORD;
	float4 TangentL	: TANGENT;
};

//! Output by the vertex shader -> input for the pixel shader.
struct VertexOut
{
	float4 PosH		: SV_POSITION;
    float3 PosW		: POSITION;
    float3 NormalW	: NORMAL;
	float4 TangentW : TANGENT;
	float2 Tex		: TEXCOORD;
	float4 ShadowPosH : TEXCOORD1;
};

//! Output for the shadow map vertex shader.
struct VertexOutShadowMap
{
	float4 PosH : SV_POSITION;
};

//! Vertex shader that transforms coordinates and normals to the world and homogeneous space.
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW     = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.TangentW = mul(vin.TangentL, gWorld);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Pass on the texture.
	vout.Tex = vin.Tex;
	
	// Generate projective tex-coords to project shadow map onto scene.
	vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);

	return vout;
}

//! Pixel shader that applies ligthing, fogging and shadow mapping.
float4 PS(VertexOut pin) : SV_Target
{
	return gTexture.Sample(textureSampler, pin.Tex);
}
//! The standard technique used by most 3D objects.
technique11 BasicTech
{
    pass P0
    {
        SetVertexShader(CompileShader( vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader( ps_5_0, PS()));
    }
}
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

cbuffer cbSkinned
{
	float4x4 gBoneTransforms[96];
};

//! The texture and normal map to use.
Texture2D gTexture;
Texture2D gNormalMap;
Texture2D gShadowMap;

//! The sampler state to use with the texture.
SamplerState textureSampler
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

//! The sampler comparison states that is used when compering Z values between the shadow map and ShadowPosH.
SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};

//! The sampler state to use with the normal map.
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
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

struct SkinnedVertexIn
{
	float3 PosL       : POSITION;
	float3 NormalL    : NORMAL;
	float2 Tex        : TEXCOORD;
	float4 TangentL   : TANGENT;
	float3 Weights    : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
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
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	
	// Generate projective tex-coords to project shadow map onto scene.
	vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);

	return vout;
}

//! Vertex shader that transforms coordinates and normals to the world and homogeneous space.
VertexOut SkinnedVS(SkinnedVertexIn vin)
{
	VertexOut vout;
	
	if(gUseAnimation)
	{
		// Init array or else we get strange warnings about SV_POSITION.
		float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		weights[0] = vin.Weights.x;
		weights[1] = vin.Weights.y;
		weights[2] = vin.Weights.z;
		weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

		float3 posL     = float3(0.0f, 0.0f, 0.0f);
		float3 normalL  = float3(0.0f, 0.0f, 0.0f);
		float3 tangentL = float3(0.0f, 0.0f, 0.0f);
		for(int i = 0; i < 4; ++i)
		{
			// Assume no nonuniform scaling when transforming normals, so 
			// that we do not have to use the inverse-transpose.
			posL     += weights[i]*mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
			normalL  += weights[i]*mul(vin.NormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
			tangentL += weights[i]*mul(vin.TangentL.xyz, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
		}

		// Transform to world space space.
		vout.PosW     = mul(float4(posL, 1.0f),  gWorld).xyz;
		vout.NormalW  = mul(normalL, (float3x3)gWorldInvTranspose);
		vout.TangentW = float4(mul(tangentL, (float3x3)gWorld), vin.TangentL.w);
		
		// Transform to homogeneous clip space.
		vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);

		// Pass on the texture.
		vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	
		// Generate projective tex-coords to project shadow map onto scene.
		vout.ShadowPosH = mul(float4(posL, 1.0f), gShadowTransform);

		return vout;
	}
	else
	{
		// Transform to world space space.
		vout.PosW     = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
		vout.NormalW  = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		vout.TangentW = mul(vin.TangentL, gWorld);
		
		// Transform to homogeneous clip space.
		vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

		// Pass on the texture.
		vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	
		// Generate projective tex-coords to project shadow map onto scene.
		vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);

		return vout;
	}
}

//! Vertex shader that transforms coordinates and normals to the world and homogeneous space.
VertexOutShadowMap ShadowVS(SkinnedVertexIn vin)
{
	VertexOutShadowMap vout;
	
	if(gUseAnimation)
	{
		// Init array or else we get strange warnings about SV_POSITION.
		float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		weights[0] = vin.Weights.x;
		weights[1] = vin.Weights.y;
		weights[2] = vin.Weights.z;
		weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

		float3 posL     = float3(0.0f, 0.0f, 0.0f);
		for(int i = 0; i < 4; ++i)
			posL     += weights[i]*mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;

		// Transform to homogeneous clip space.
		vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	}
	else
		vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	return vout;
}

//! Pixel shader that applies ligthing, fogging and shadow mapping.
float4 PS(VertexOut pin) : SV_Target
{
	if(gRenderingToShadowMap)
		return float4(1.0f, 1.0f, 1.0f, 1.0f);

	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW); 

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Identity as default.
	float4 texColor = float4(1, 1, 1, 1);
	if(gUseTexture)
		texColor = gTexture.Sample(textureSampler, pin.Tex);

	// Get the shadow factor.
	float shadow = 1.0f;
	//shadow = CalcShadowFactor(samShadow, gShadowMap, pin.ShadowPosH);

	// Apply lighting.
	float4 litColor;
	if(gUseLighting)
		ApplyLighting(gNumLights, gLights, gMaterial, pin.PosW, pin.NormalW, toEyeW, texColor, shadow, litColor);
	else
		litColor = texColor*(gMaterial.ambient + gMaterial.diffuse) + gMaterial.specular;

	//! Apply fogging.
	float distToEye = length(gEyePosW - pin.PosW);
	float fogLerp = saturate((distToEye - gFogStart) / gFogRange); 

	// Blend the fog color and the lit color.
	litColor = lerp(litColor, gFogColor, fogLerp);

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.diffuse.a * texColor.a;

    return litColor;
}

//! Pixel shader that applies ligthing, fogging, normal mapping and shadow mapping.
float4 NormalMapPS(VertexOut pin) : SV_Target
{
	if(gRenderingToShadowMap)
		return float4(1.0f, 1.0f, 1.0f, 1.0f);

	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW); 

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Identity as default.
	float4 texColor = float4(1, 1, 1, 1);

	if(gUseTexture)
		texColor = gTexture.Sample(textureSampler, pin.Tex);

	float3 normalMapSample = gNormalMap.Sample(samLinear, pin.Tex).rgb;
	pin.NormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW);

	// Get the shadow factor.
	float shadow = 1.0f;
	shadow = CalcShadowFactor(samShadow, gShadowMap, pin.ShadowPosH);

	// Apply lighting.
	float4 litColor;
	if(gUseLighting)
		ApplyLighting(gNumLights, gLights, gMaterial, pin.PosW, pin.NormalW, toEyeW, texColor, shadow, litColor);
	else
		litColor = texColor*(gMaterial.ambient + gMaterial.diffuse) + gMaterial.specular;

	//! Apply fogging.
	float distToEye = length(gEyePosW - pin.PosW);
	float fogLerp = saturate((distToEye - gFogStart) / gFogRange); 

	// Blend the fog color and the lit color.
	litColor = lerp(litColor, gFogColor, fogLerp);

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.diffuse.a * texColor.a;

    return litColor;
}

//! The standard technique used by most 3D objects.
technique11 BasicTech
{
    pass P0
    {
        SetVertexShader(CompileShader( vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader( ps_5_0, PS()));
    }
}

//! Skinned mesh technique.
technique11 SkinnedTech
{
    pass P0
    {
        SetVertexShader(CompileShader( vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader( ps_5_0, PS()));
    }
}

//! Normal map technique.
technique11 NormalMapTech
{
    pass P0
    {
        SetVertexShader(CompileShader( vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader( ps_5_0, NormalMapPS()));
    }
}

RasterizerState Depth
{
	// To remove shadow acne.
	DepthBias = 100000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

RasterizerState SolidMode
{
	FillMode = Solid;
};

//! Shadow map technique.
//! Used when rendering to the shadow map.
technique11 ShadowMapTech
{
    pass P0
    {
        SetVertexShader(CompileShader( vs_5_0, ShadowVS()));
		SetGeometryShader(NULL);
        SetPixelShader(NULL);
		SetRasterizerState(SolidMode);
    }
}
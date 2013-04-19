 #include "LightHelper.fx"
 
/*
	This effect is used when rendering the terrain.
	It's very similar to Basic.fx.
*/

cbuffer cbPerFrame
{
	// Lights & num lights.
	Light	gLights[10];
	float	gNumLights;

	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
	
	float gTexelCellSpaceU;
	float gTexelCellSpaceV;
	float gWorldCellSpace;
	float gTexScale = 1.0f;	// Default = 1.0f
};

cbuffer cbPerObject
{
	// World matrix is identity.
	float4x4 gViewProj;
	float4x4 gShadowTransform; 
	Material gMaterial;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2DArray gLayerMapArray;
Texture2D gBlendMap;
Texture2D gHeightMap;
Texture2D gShadowMap;

//! Used when sampling the layers and blend map.
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

//! Used when sampling the height map.
SamplerState samHeightmap
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
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

//! Input to the vertex shader.
struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 Tex		: TEXCOORD;
	float3 TangentL	: TANGENT;
};

//! Output from the vertex shader -> input for the pixel shader.
struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW     : POSITION;
	float2 Tex      : TEXCOORD0;
	float4 ShadowPosH : TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Terrain specified directly in world space.
	vin.PosL.y = gHeightMap.SampleLevel( samHeightmap, vin.Tex, 0 ).r;
	vout.PosW = vin.PosL;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gViewProj);

	// Pass on the texture.
	vout.Tex = vin.Tex;

	// Generate projective tex-coords to project shadow map onto scene.
	vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Estimate normal and tangent using central differences.
	float2 leftTex   = pin.Tex + float2(-gTexelCellSpaceU, 0.0f);
	float2 rightTex  = pin.Tex + float2(gTexelCellSpaceU, 0.0f);
	float2 bottomTex = pin.Tex + float2(0.0f, gTexelCellSpaceV);
	float2 topTex    = pin.Tex + float2(0.0f, -gTexelCellSpaceV);
	
	float leftY   = gHeightMap.SampleLevel( samHeightmap, leftTex, 0 ).r;
	float rightY  = gHeightMap.SampleLevel( samHeightmap, rightTex, 0 ).r;
	float bottomY = gHeightMap.SampleLevel( samHeightmap, bottomTex, 0 ).r;
	float topY    = gHeightMap.SampleLevel( samHeightmap, topTex, 0 ).r;
	
	float3 tangent = normalize(float3(2.0f*gWorldCellSpace, rightY - leftY, 0.0f));
	float3 bitan   = normalize(float3(0.0f, bottomY - topY, -2.0f*gWorldCellSpace)); 
	float3 normalW = cross(tangent, bitan);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
	/* Texturing */

	// Scale the textures by gTexScale.
	float2 scaledTex = pin.Tex * gTexScale;

	// Sample layers in texture array.
	float4 c0 = gLayerMapArray.Sample( samLinear, float3(scaledTex, 0.0f) );
	float4 c1 = gLayerMapArray.Sample( samLinear, float3(scaledTex, 1.0f) );
	float4 c2 = gLayerMapArray.Sample( samLinear, float3(scaledTex, 2.0f) );
	float4 c3 = gLayerMapArray.Sample( samLinear, float3(scaledTex, 3.0f) );
	float4 c4 = gLayerMapArray.Sample( samLinear, float3(scaledTex, 4.0f) ); 
	
	// Sample the blend map.
	float4 t  = gBlendMap.Sample( samLinear, pin.Tex ); 
    
    // Blend the layers on top of each other.
    float4 texColor = c0;
	texColor = lerp(texColor, c1, t.r);
	texColor = lerp(texColor, c2, t.g);
	texColor = lerp(texColor, c3, t.b);
	texColor = lerp(texColor, c4, t.a);

	// Get the shadow factor.
	float shadow = 1.0f;
	//shadow = CalcShadowFactor(samShadow, gShadowMap, pin.ShadowPosH);

	// Apply lighting.
	float4 litColor;
	ApplyLighting(gNumLights, gLights, gMaterial, pin.PosW, normalW, toEye, texColor, shadow, litColor);

	//! Apply fogging.
	float fogLerp = saturate((distToEye - gFogStart) / gFogRange); 

	// Blend the fog color and the lit color.
	litColor = lerp(litColor, gFogColor, fogLerp);

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.diffuse.a * texColor.a;

    return litColor;
}

technique11 TerrainTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
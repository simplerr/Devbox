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

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4	 gColor;
	Material gMaterial;
}; 

struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL	: NORMAL;
	int Type		: SIZE;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;	
	float3 NormalW	: NORMAL;
	int Type		: SIZE;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW     = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.Type = vin.Type;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW); 

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Type?
	float4 texColor;
	if(pin.Type == 0)		// Grass
	{
		texColor = float4(0, 1, 0, 1);
	}
	else if(pin.Type == 1)	// Dirt
	{
		texColor = float4(1, 0, 0, 1);
	}
	else if(pin.Type == 2)	// Water
	{
		texColor = float4(0, 0, 1, 1);
	}

	// Apply lighting.
	float4 litColor;
	float shadow = 1.0f;
	ApplyLighting(gNumLights, gLights, gMaterial, pin.PosW, pin.NormalW, toEyeW, texColor, shadow, litColor);

	//! Apply fogging.
	float distToEye = length(gEyePosW - pin.PosW);
	float fogLerp = saturate((distToEye - gFogStart) / gFogRange); 

	// Blend the fog color and the lit color.
	litColor = lerp(litColor, gFogColor, fogLerp);

	/**
		2nd fog effect.
	*/

	fogLerp = min(saturate(25.0f / distToEye), 0.3); 
	litColor = lerp(litColor, float4(0, 0.3, 0, 1), fogLerp);

	fogLerp = min(saturate(10.0f / distToEye), 0.3); 
	litColor = lerp(litColor, float4(0, 0.3, 0, 1), fogLerp);

	fogLerp = min(saturate(5.0f / distToEye), 0.3); 
	litColor = lerp(litColor, float4(0, 0.3, 0, 1), fogLerp);

	fogLerp = min(saturate(1.0f / distToEye), 0.3); 
	litColor = lerp(litColor, float4(0, 0.3, 0, 1), fogLerp);

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.diffuse.a * texColor.a;

    return litColor;
}

RasterizerState SolidMode
{
	FillMode = Solid;
};

technique11 VoxelTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
		SetGeometryShader(NULL);
		SetRasterizerState(SolidMode);
    }
}
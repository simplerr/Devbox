// Effect used to build the shadow map.

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4	 gColor;
}; 

struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL	: NORMAL;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float4 Color	: COLOR;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	float3 posW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	float3 normalW  = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	float3 lightDir = normalize(float3(0.5f, 1, 0.5f));
	float diffuseFactor = max(dot(lightDir, vin.NormalL), 0);

	float4 ambient = gColor * 0.4;
	ambient.a = 1.0; 

	float4 diffuse = gColor * 0.7;
	diffuse.a = 1.0;
	vout.Color = ambient + diffuseFactor * diffuse;
	vout.Color.a = 1.0f;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
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
        SetPixelShader(CompileShader( ps_5_0, PS()));

		SetRasterizerState(SolidMode);
    }
}
// Effect used to build the shadow map.

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
	float4	 gColor;
}; 
 
struct VertexIn
{
	float3 PosL     : POSITION;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gColor;
}

RasterizerState WireframeRasterizer
{
	CullMode = NONE;
	FillMode = Wireframe;
};

RasterizerState SolidRasterizer
{
	CullMode = NONE;
	FillMode = Solid;
};

technique11 SolidTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader( ps_5_0, PS()));

		SetRasterizerState(SolidRasterizer);
    }
}

technique11 WireframeTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader( ps_5_0, PS()));

		SetRasterizerState(WireframeRasterizer);
    }
}
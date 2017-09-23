
struct VSOut
{
	float4 PositionW : POSITION0;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
	float2 Texture : TEXCOORD;
	float Tessfactor : TESSFACTOR;
};


cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
    float3 gEyePos;
    float pad;
}

VSOut main(float3 pos : POSITION, float3 nor : NORMAL, float3 bin : BINORMAL, float3 tan : TANGENT, float2 tex : TEXCOORD)
{
	VSOut output = (VSOut) 0;

    output.PositionW = mul(float4(pos, 1.0f), gWorld);
    output.TangentW = mul(tan, (float3x3) gWorld);
    output.NormalW = mul(nor, (float3x3) gWorld);
    output.BinormalW = mul(bin, (float3x3) gWorld);

    output.Texture = tex;

    float dist = distance(output.PositionW.xyz, gEyePos);

    static const float gMinTess = 1.0f;
    static const float gMaxTess = 10.0f;

    static const float gMinTessDistance = 0.0f;
    static const float gMaxTessDistance = 100.0f;

    float tess = saturate((gMaxTessDistance - dist) / (gMaxTessDistance - gMinTessDistance));

    output.Tessfactor = gMinTess + tess * (gMaxTess - gMinTess);

	return output;
}
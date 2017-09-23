
Texture2D ObjTexture : register(t0);
Texture2D ObjBumpmap : register(t1);
SamplerState WrapSampler : register(s0);

struct PSIn
{
	float4 PositionH : SV_POSITION;
	float4 PositionW : POSITION0;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
	float2 Texture : TEXCOORD;
};

cbuffer cbPerLight : register(b0)
{
	float4 Diffuse;
	float4 Ambient;
	float3 LightDir;
	float pad;
}

float3 NormalSampleToWorldSpace(float3 Sample, float3 Normal, float3 Tangent, float3 Binormal)
{
	float3 normalT = 2.0f * Sample - 1.0f;

	float3 bumpedNormal = mul(normalT, float3x3(Normal, Tangent - Normal * dot(Normal, Tangent), Binormal));
	return bumpedNormal;
}

float4 main(PSIn input) : SV_TARGET
{
	input.NormalW = normalize(input.NormalW);

	float4 TexColor = ObjTexture.Sample(WrapSampler, input.Texture);

	float4 Color = Ambient;

	float3 NormalMapSample = ObjBumpmap.Sample(WrapSampler, input.Texture).rgb;
	NormalMapSample = NormalSampleToWorldSpace(NormalMapSample, input.NormalW, input.TangentW, input.BinormalW);

	float howMuchLight = dot(NormalMapSample, -LightDir);
	if (howMuchLight > 0.0f)
	{
		Color += float4(1.0f, 1.0f, 1.0f, 1.0f) * howMuchLight;
	}

	Color = saturate(Color);

	return Color * TexColor;
}
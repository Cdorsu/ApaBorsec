struct DS_OUTPUT
{
	float4 PositionH : SV_POSITION;
	float4 PositionW : POSITION0;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
	float2 Texture : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 PositionW : POSITION0;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
	float2 Texture : TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

cbuffer cbPerObject : register(b0)
{
	float4x4 gViewProjection;
	float3 gEyePos;
};

Texture2D DisplacementMap : register(t0);
SamplerState WrapSampler : register(s0);


[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.PositionW = domain.x * patch[0].PositionW + domain.y * patch[1].PositionW + domain.z * patch[2].PositionW;
	Output.BinormalW = domain.x * patch[0].BinormalW + domain.y * patch[1].BinormalW + domain.z * patch[2].BinormalW;
	Output.NormalW = domain.x * patch[0].NormalW + domain.y * patch[1].NormalW + domain.z * patch[2].NormalW;
	Output.TangentW = domain.x * patch[0].TangentW + domain.y * patch[1].TangentW + domain.z * patch[2].TangentW;
	Output.Texture = domain.x * patch[0].Texture + domain.y * patch[1].Texture + domain.z * patch[2].Texture;

	Output.NormalW = normalize(Output.NormalW);

	static const float MipInterval = 20.0f;

	float Dist = distance(Output.PositionW.xyz, gEyePos);

	float mipLevel = clamp((Dist - MipInterval) / MipInterval, 0.0f, 6.0f);

	float h = DisplacementMap.SampleLevel(WrapSampler, Output.Texture, mipLevel).a;

	static const float gHeightScale = 0.2f;
	Output.PositionW += float4((gHeightScale * (1.0f - h)) * -Output.NormalW, 0.0f);

	Output.PositionH = mul(Output.PositionW, gViewProjection);


	return Output;
}

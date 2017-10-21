
Texture2D Heightmap : register(t0);
SamplerState Sampler : register(s0);

cbuffer cbPerObject
{
    float4x4 WorldViewProjection;
};

struct DS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float2 Tex : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
    float4 PosW : SV_POSITION;
    float2 Tex : TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor[2]		: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 4

[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

    float4 Position = lerp(
		lerp(patch[0].PosW, patch[1].PosW, domain.x),
		lerp(patch[2].PosW, patch[3].PosW, domain.x),
		domain.y);
	
    float2 Texture = lerp(
		lerp(patch[0].Tex, patch[1].Tex, domain.x),
		lerp(patch[2].Tex, patch[3].Tex, domain.x),
		domain.y);

    Output.PosW = Position;
    Output.Tex = Texture;

    float height = Heightmap.SampleLevel(Sampler, Output.Tex, 0).b * 20.0f;

    Output.PosW.y = height;
    Output.PosH = mul(Output.PosW, WorldViewProjection);

	return Output;
}

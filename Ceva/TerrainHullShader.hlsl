// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    float4 PosW : SV_POSITION;
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
	float InsideTessFactor[2]		: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domai
};

#define NUM_CONTROL_POINTS 4

cbuffer cbPerFrame : register(b0)
{
    float3 gEyePos;
    float pad;
};

static const float MinTessDistance = 20.0f;
static const float MaxTessDistance = 1000.0f;
static const float MinTess = 0.0f;
static const float MaxTess = 6.0f;

float CalculateTesselation(float3 p)
{
    float dist = distance(p, gEyePos);
    float s = saturate((dist - MinTessDistance) / (MaxTessDistance - MinTessDistance));
	
    return pow(2, lerp(MaxTess, MinTess, s));
}

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

    float4 e0 = 0.5f * (ip[0].PosW + ip[2].PosW);
    float4 e1 = 0.5f * (ip[0].PosW + ip[1].PosW);
    float4 e2 = 0.5f * (ip[1].PosW + ip[3].PosW);
    float4 e3 = 0.5f * (ip[2].PosW + ip[3].PosW);
    float4 c = 0.25f * (ip[0].PosW + ip[1].PosW + ip[2].PosW + ip[3].PosW);

    Output.EdgeTessFactor[0] = CalculateTesselation((float3) e0);
    Output.EdgeTessFactor[1] = CalculateTesselation((float3) e1);
    Output.EdgeTessFactor[2] = CalculateTesselation((float3) e2);
    Output.EdgeTessFactor[3] = CalculateTesselation((float3) e3);

    Output.InsideTessFactor[0] = CalculateTesselation((float3) c);
    Output.InsideTessFactor[1] = Output.InsideTessFactor[0];

	return Output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	// Insert code to compute Output here
    Output.PosW = ip[i].PosW;
    Output.Tex = ip[i].Tex;

	return Output;
}

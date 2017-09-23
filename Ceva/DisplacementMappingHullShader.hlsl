// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
	float4 PositionW : POSITION0;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
	float2 Texture : TEXCOORD;
	float Tessfactor : TESSFACTOR;
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

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	Output.EdgeTessFactor[0] = 0.5f * (ip[1].Tessfactor + ip[2].Tessfactor);
	Output.EdgeTessFactor[1] = 0.5f * (ip[0].Tessfactor + ip[2].Tessfactor);
	Output.EdgeTessFactor[2] = 0.5f * (ip[1].Tessfactor + ip[0].Tessfactor);

	Output.InsideTessFactor = Output.EdgeTessFactor[0];

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	Output.PositionW = ip[i].PositionW;
	Output.BinormalW = ip[i].BinormalW;
	Output.NormalW = ip[i].NormalW;
	Output.TangentW = ip[i].TangentW;
	Output.Texture = ip[i].Texture;

	return Output;
}

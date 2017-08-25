struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : POSITION; 
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor[2]		: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

cbuffer cbPerObject
{
	float4x4 WVP;
};

#define NUM_CONTROL_POINTS 4

[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	float3 v1 = lerp(patch[0].vPosition, patch[1].vPosition, domain.x);
	float3 v2 = lerp(patch[2].vPosition, patch[3].vPosition, domain.x);
	float3 v3 = lerp(v1, v2, domain.y);

    float displacement = 0.3f * (2.0f * sin(v3.z) + 3.0f * cos(v3.x));
    v3.y = displacement;

	Output.vPosition = float4(v3, 1.0f);
    Output.vPosition = mul(Output.vPosition, WVP);

	return Output;
}

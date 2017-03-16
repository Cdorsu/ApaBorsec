
struct VSOut
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float clipDistance : SV_ClipDistance0;
};

cbuffer cbPerObject : register ( b0 )
{
	float4x4 WVP;
	float4x4 World;
	float4 clipPlane;
};

VSOut main( float4 pos : POSITION, float2 tex : TEXCOORD, float3 nor : NORMAL )
{
	VSOut output;
	output.pos = mul( pos, WVP );
	output.tex = tex;
	output.worldPos = mul( pos, World ).xyz;
	output.normal = mul( nor, ( float3x3 )World );
	output.normal = normalize( output.normal );
	output.clipDistance = dot( mul( pos, World ), clipPlane );

	return output;
}
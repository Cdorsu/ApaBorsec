
struct VSOut
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

cbuffer cbPerObject : register ( b0 )
{
	float4x4 WVP;
	float4x4 World;
};

VSOut main( float4 pos : POSITION, float2 tex : TEXCOORD )
{
	VSOut output;
	output.pos = mul( pos, WVP );
	output.tex = tex;
	return output;
}
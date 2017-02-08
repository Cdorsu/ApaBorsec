
struct VSOut
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 refractionPosition : TEXCOORD1;
	float4 reflectionPosition : TEXCOORD2;
};

cbuffer cbPerObject : register ( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 Reflection;
};

VSOut main( float4 pos : POSITION, float2 tex : TEXCOORD, float3 nor : NORMAL )
{
	float4x4 reflectProjectionWorld;
	float4x4 viewProjectionWorld;
	VSOut output;

	output.pos = mul( pos, World );
	output.pos = mul( output.pos, View );
	output.pos = mul( output.pos, Projection );
	
	output.tex = tex;

	reflectProjectionWorld = mul( Reflection, Projection );
	reflectProjectionWorld = mul( World, reflectProjectionWorld );
	output.reflectionPosition = mul( pos, reflectProjectionWorld );

	viewProjectionWorld = mul( View, Projection );
	viewProjectionWorld = mul( World, viewProjectionWorld );
	output.refractionPosition = mul( pos, viewProjectionWorld );

	return output;
}
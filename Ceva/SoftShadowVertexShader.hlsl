
cbuffer cbMatrices : register( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

cbuffer cbLight : register( b1 )
{
	float3 LightPos;
};

struct VSOut
{
	float4 Position : SV_Position;
	float4 WorldPos : POSITION0;
	float3 LightPos : POSITION1;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

VSOut main ( float4 inPos : POSITION, float3 inNor : NORMAL, float2 inTex : TEXCOORD )
{
	VSOut output = ( VSOut ) 0;
	inPos.w = 1.0f;
	output.WorldPos = mul ( inPos, World );
	
	output.Position = mul ( inPos, World );
	output.Position = mul ( output.Position, View );
	output.Position = mul ( output.Position, Projection );

	output.WorldPos = output.Position;

	output.LightPos = LightPos.xyz - output.WorldPos.xyz;
	output.LightPos = normalize ( output.LightPos );

	output.Normal = mul ( inNor, ( float3x3 ) World );
	output.Normal = normalize ( output.Normal );

	output.TexCoord = inTex;

	return output;
}
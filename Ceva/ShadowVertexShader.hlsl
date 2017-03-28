
cbuffer cbMatrices : register( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 LightView;
	float4x4 LightProjection;
};

cbuffer cbLight : register( b1 )
{
	float3 LightPos;
};

struct VSOut
{
	float4 Position : SV_Position;
	float4 WorldPos : POSITION0;
	float4 LightViewProjection : POSITION1;
	float3 lightPos : POSITION2;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

VSOut main( float4 inPos : POSITION, float2 inTex : TEXCOORD, float3 inNor : NORMAL )
{
	VSOut output = ( VSOut ) 0;

	inPos.w = 1.0f;

	output.WorldPos = mul ( inPos, World );

	output.Position = mul ( inPos, World );
	output.Position = mul ( output.Position, View );
	output.Position = mul ( output.Position, Projection );

	output.TexCoord = inTex;
	
	output.Normal = mul ( inNor, ( float3x3 ) World );
	output.Normal = normalize ( output.Normal );

	output.LightViewProjection = mul ( inPos, World );
	output.LightViewProjection = mul ( output.LightViewProjection, LightView );
	output.LightViewProjection = mul ( output.LightViewProjection, LightProjection );

	//output.lightPos = output.WorldPos.xyz - LightPos.xyz;
	output.lightPos = LightPos.xyz - output.WorldPos.xyz;
	output.lightPos = normalize ( output.lightPos );

	return output;
}
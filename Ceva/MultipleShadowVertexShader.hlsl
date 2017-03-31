
cbuffer cbMatrices : register( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 LightView1;
	float4x4 LightProjection1;
	float4x4 LightView2;
	float4x4 LightProjection2;
}

cbuffer cbLight : register( b1 )
{
	float3 LightPos1;
	float3 LightPos2;
}

struct VSOut
{
	float4 Position : SV_Position;
	float4 WorldPos : POSITION0;
	float4 LightView1 : POSITION1;
	float4 LightView2 : POSITION2;
	float3 LightPosition1 : POSITION3;
	float3 LightPosition2 : POSITION4;
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

	output.LightView1 = mul ( inPos, World );
	output.LightView1 = mul ( output.LightView1, LightView1 );
	output.LightView1 = mul ( output.LightView1, LightProjection1 );

	output.LightView2 = mul ( inPos, World );
	output.LightView2 = mul ( output.LightView2, LightView2 );
	output.LightView2 = mul ( output.LightView2, LightProjection2 );

	output.LightPosition1 = LightPos1.xyz - output.WorldPos.xyz;
	output.LightPosition1 = normalize ( output.LightPosition1 );

	output.LightPosition2 = LightPos2.xyz - output.WorldPos.xyz;
	output.LightPosition2 = normalize ( output.LightPosition2 );

	output.Normal = mul ( inNor, ( float3x3 ) World );
	output.Normal = normalize ( output.Normal );

	output.TexCoord = inTex;


	return output;
}
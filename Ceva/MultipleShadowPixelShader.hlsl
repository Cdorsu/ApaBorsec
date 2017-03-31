
Texture2D ObjTexture : register( t0 );
Texture2D ObjDepthmap1 : register( t1 );
Texture2D ObjDepthmap2 : register( t2 );

SamplerState WrapSampler : register( s0 );
SamplerState ClampSampler : register( s1 );


cbuffer cbLight : register( b0 )
{
	float4 Diffuse1;
	float4 Ambient1;
	float4 Diffuse2;
	float4 Ambient2;
};

struct PSIn
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

float4 ApplyShadow ( float4 LightViewProj, float4 Diffuse, uint depthmapIndex, float3 Normal, float3 PixelToLightVec, float bias = 0.001f )
{
	float4 color = float4 ( 0.0f, 0.0f, 0.0f, 0.0f );
	float2 projectedLight;
	projectedLight.x = LightViewProj.x / LightViewProj.w / 2.0f + 0.5f;
	projectedLight.y = -LightViewProj.y / LightViewProj.w / 2.0f + 0.5f;
	
	if ( projectedLight.x == saturate ( projectedLight.x ) && projectedLight.y == saturate ( projectedLight.y ) )
	{
		float depth = 0.0f;
		if ( depthmapIndex == 1 )
			depth = ObjDepthmap1.Sample ( ClampSampler, projectedLight ).r;
		else if ( depthmapIndex == 2 )
			depth = ObjDepthmap2.Sample ( ClampSampler, projectedLight ).r;
		float lightDepth = LightViewProj.z / LightViewProj.w;
		lightDepth -= bias;
		if ( depth > lightDepth )
		{
			float howMuchLight = saturate ( dot ( Normal, PixelToLightVec ) );
			if ( howMuchLight > 0.0f )
				return Diffuse * howMuchLight;
		}
	}
	return color;
}

float4 main ( PSIn input ) : SV_TARGET
{
	float4 color1, color2, finalColor;
	float4 textureColor = ObjTexture.Sample ( WrapSampler, input.TexCoord );
	float bias = 0.001f;
	float2 projectedLight1, projectedLight2;

	color1 = ApplyShadow ( input.LightView1, Diffuse1, 1, input.Normal, input.LightPosition1 ); 

	color2 = ApplyShadow ( input.LightView2, Diffuse2, 2, input.Normal, input.LightPosition2 );

	finalColor = color1 + color2 + Ambient1 + Ambient2;
	finalColor = saturate ( finalColor );
	
	return finalColor * textureColor;
	
}
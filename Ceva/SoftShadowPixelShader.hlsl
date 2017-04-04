
Texture2D ObjTexture : register( t0 );
Texture2D ObjShadowTexture : register( t1 );

SamplerState WrapSampler : register( s0 );
SamplerState ClampSample : register( s1 );

cbuffer cbLight : register( b0 )
{
	float4 Diffuse;
	float4 Ambient;
};

struct PSIn
{
	float4 Position : SV_Position;
	float4 WorldPos : POSITION0;
	float3 LightPos : POSITION1;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

float4 main ( PSIn input ) : SV_TARGET
{
	float4 color = float4 ( 0.0f, 0.0f, 0.0f, 0.0f );
	float4 textureColor = ObjTexture.Sample ( WrapSampler, input.TexCoord );
	float2 projectedToView;
	float shadowValue;

	float howMuchLight = dot ( input.Normal, input.LightPos );
	if ( howMuchLight > 0.0f )
	{
		color = Diffuse * howMuchLight;
		color = saturate ( color );
	}
	color = color * textureColor;

	//return color;

	projectedToView.x = input.WorldPos.x / input.WorldPos.w / 2.0f + 0.5f;
	projectedToView.y = -input.WorldPos.y / input.WorldPos.w / 2.0f + 0.5f;

	shadowValue = ObjShadowTexture.Sample ( ClampSample, projectedToView ).r;

	color = color * shadowValue;
	
	color += Ambient * textureColor;
	saturate( color );


	return color;
}
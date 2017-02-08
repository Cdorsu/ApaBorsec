SamplerState Sampler : register ( s0 );
Texture2D ObjTexture : register ( t0 );

struct PSIn
{
	float4 Pos : SV_Position;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

cbuffer cbLight : register ( b0 )
{
	float4 Diffuse;
	float4 Ambient;
	float3 Direction;
};

float4 main( PSIn input ) : SV_TARGET
{
	//return ObjTexture.Sample( Sampler, input.Tex );
	float4 textureColor = ObjTexture.Sample( Sampler,input.Tex );
	float4 color = Ambient;
	float3 lightDir = -Direction;

	float howMuchLight = dot( input.Normal, lightDir );
	if ( howMuchLight > 0.0f )
	{
		color += ( Diffuse * howMuchLight );
	}
	color = saturate( color );
	textureColor = color * textureColor;
	return textureColor;
}


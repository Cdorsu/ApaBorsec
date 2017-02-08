Texture2D normals : register ( t0 );
Texture2D reflection : register ( t1 );
Texture2D refraction : register ( t2 );

SamplerState Sampler : register ( s0 );

cbuffer cbWaterInfo : register ( b0 )
{
	float textureTranslation;
	float reflectRefractScale;
}

struct PSIn
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 refractionPosition : TEXCOORD1;
	float4 reflectionPosition : TEXCOORD2;
};

float4 main( PSIn input ) : SV_TARGET
{
	float4 reflectionColor;
	float4 refractionColor;
	input.tex.y += textureTranslation;
	float3 normal = normals.Sample( Sampler,input.tex ).xyz;
	normal = ( normal * 2.0f ) - 1.0f;

	float2 reflectTexCoord;
	float2 refractTexCoord;

	reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
	reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

	refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
	refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

	refractTexCoord += normal.xy * reflectRefractScale;
	reflectTexCoord += normal.xy * reflectRefractScale;

	reflectionColor = reflection.Sample( Sampler, reflectTexCoord );
	refractionColor = refraction.Sample( Sampler, refractTexCoord );
	//return normals.Sample( Sampler, input.tex );
	return lerp( reflectionColor, refractionColor, 0.6f );
}


TextureCube Texture : register(t0);
SamplerState WrapSampler : register(s0);

struct PSIn
{
	float4 Position : SV_POSITION;
    float4 PositionW : POSITION;
	float3 TexCoords : TEXCOORD;
	float3 Normal : NORMAL;
};

cbuffer perFrame : register(b0)
{
	float3 eyePos;
};

float4 main(PSIn input) : SV_TARGET
{
    input.Normal = normalize( input.Normal );
	float3 incident = input.PositionW.xyz - eyePos;
	float3 reflection = reflect(incident, input.Normal);
    float3 refraction = refract( incident, input.Normal, 0.33f );
    return Texture.Sample( WrapSampler, reflection );
}
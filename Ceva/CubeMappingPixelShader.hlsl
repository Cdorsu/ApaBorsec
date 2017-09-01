

TextureCube Texture : register(t0);
SamplerState WrapSampler : register(s0);

struct PSIn
{
	float4 Position : SV_POSITION;
	float3 TexCoords : TEXCOORDS;
	float3 Normal : NORMAL;
};

cbuffer perFrame : register(b0)
{
	float3 eyePos;
};

float4 main(PSIn input) : SV_TARGET
{
	float3 incident = input.Position.xyz - eyePos;
	float3 reflection = reflect(incident, input.Normal);
	return Texture.Sample(WrapSampler, reflection);
}
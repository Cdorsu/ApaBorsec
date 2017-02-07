SamplerState Sampler;
Texture2D ObjTexture;

struct PSIn
{
	float4 Pos : SV_Position;
	float2 Tex : TEXCOORD;
	float3 Nor : NORMAL;
	float3 ViewDirection : POSITION;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
};

cbuffer cbFadding
{
	float fade;
};

float4 main( PSIn input ) : SV_TARGET
{
	return ObjTexture.Sample( Sampler,input.Tex ) * fade;
}


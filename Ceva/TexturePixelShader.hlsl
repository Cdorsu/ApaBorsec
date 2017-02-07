SamplerState Sampler : register ( s0 );
Texture2D ObjTexture : register ( t0 );

struct PSIn
{
	float4 Pos : SV_Position;
	float2 Tex : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
	return ObjTexture.Sample( Sampler, input.Tex );
}


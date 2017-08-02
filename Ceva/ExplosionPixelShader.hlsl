SamplerState Sampler : register(s0);
Texture2D ObjTexture : register(t0);

struct PSIn
{
    float4 pos : SV_POSITION;
    float2 Texture : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
    return ObjTexture.Sample(Sampler, input.Texture);
}

SamplerState Sampler;
Texture2DArray Textures;

struct PSIn
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

float4 main(PSIn input) : SV_TARGET
{
    float4 Color = Textures.Sample(Sampler, float3(input.tex.x, input.tex.y, 0));
    clip(Color.a - 0.1f);
    return Color;
}
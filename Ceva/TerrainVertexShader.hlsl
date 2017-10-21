
Texture2D Heightmap : register(t0);
SamplerState Sampler : register(s0);

struct VSOut
{
    float4 PosW : SV_POSITION;
    float2 Tex : TEXCOORD;
};


VSOut main(float3 pos : POSITION, float2 tex : TEXCOORD)
{
    VSOut output;
    output.Tex = tex;
    output.PosW = float4(pos, 1.0f);

    output.PosW.y = Heightmap.SampleLevel(Sampler, output.Tex, 0).b * 10.0f;

    return output;
}
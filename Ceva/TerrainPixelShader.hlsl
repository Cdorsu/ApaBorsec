
Texture2D Heightmap : register(t0);
SamplerState Sampler : register(s0);

cbuffer cbPerTerrain : register(b0)
{
    float gTexCellSpacingU;
    float gTexCellSpacingV;
    float2 pad;
};

cbuffer cbLight : register(b1)
{
    float4 Diffuse;
    float4 Ambient;
    float3 LightDir;
    float pad1;
};

struct PSIn
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float2 Tex : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
    float2 leftUV = input.Tex + float2(-gTexCellSpacingU, 0.0f);
    float2 rightUV = input.Tex + float2(gTexCellSpacingU, 0.0f);
    float2 topUV = input.Tex + float2(0.0f, -gTexCellSpacingV);
    float2 bottomUV = input.Tex + float2(0.0f, gTexCellSpacingV);

    float leftY = Heightmap.SampleLevel(Sampler, leftUV, 0).b;
    float rightY = Heightmap.SampleLevel(Sampler, rightUV, 0).b;
    float topY = Heightmap.SampleLevel(Sampler, topUV, 0).b;
    float bottomY = Heightmap.SampleLevel(Sampler, bottomUV, 0).b;

    float3 tangent = normalize(float3(1.0f, rightY - leftY, 0.0f));
    float3 binormal = normalize(float3(0.0f, bottomY - topY, -1.0f));
    float3 normal = cross(tangent, binormal);

    float4 Green = float4(0.0f, 1.0f, 0.0f, 1.0f);
    float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    Color += Ambient;
    float howMuchLight = dot(normal, -LightDir);
    if (howMuchLight > 0.0f)
    {
        Color += howMuchLight * Diffuse;
    }
    Color *= Green;
    return Color;
}
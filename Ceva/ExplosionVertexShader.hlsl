
struct VSOut
{
    float4 Position : SV_Position;
    float2 Texture : TEXCOORD;
    float3 Normal : NORMAL;
};

VSOut main(float4 pos : POSITION, float2 tex : TEXCOORD, float3 nor : NORMAL)
{
    VSOut output;
    output.Position = pos;
    output.Texture = tex;
    output.Normal = nor;
    return output;
}
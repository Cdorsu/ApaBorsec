
struct VSOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

cbuffer cbPerobject : register(b0)
{
    float4x4 WVP;
};

VSOut main(float4 pos : POSITION, float3 color : INSTANCECOLOR, float3 position : INSTANCEPOS)
{
    VSOut output = (VSOut) 0;
    pos.xyz += position;
    
    output.position = mul(pos, WVP);
    output.color.xyz = color;

    return output;
}
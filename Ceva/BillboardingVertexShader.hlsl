
cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
    float4x4 World;
};

struct VSOut
{
    float4 position : SV_Position;
    float4 worldPos : POSITION;
    float4x4 world : TEXCOORD;
    float4x4 WVP : POSITION1;
};

VSOut main( float4 pos : POSITION )
{
    VSOut output = (VSOut)0;
    output.position = pos;
    output.WVP = WVP;
    return output;
}
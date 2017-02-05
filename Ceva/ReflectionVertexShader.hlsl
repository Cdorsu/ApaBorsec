
struct VSOut
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD;
    float4 Reflection : TEXCOORD1;
};

cbuffer cbPerObject
{
    float4x4 WVP;
    float4x4 RVP;
    float4x4 World;
};

VSOut main( float4 pos : POSITION, float2 Texture : TEXCOORD )
{
    float4 WorldPos = mul( pos, World );
    VSOut output = (VSOut)0;
    output.Pos = mul( pos, WVP );
    output.Tex = Texture;
    //output.Reflection = mul( pos, RVP );


    return output;
}
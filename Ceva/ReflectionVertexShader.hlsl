
struct VSOut
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD;
    float4 Reflection : TEXCOORD1;
};

cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4x4 ReflectView;
};

VSOut main( float4 pos : POSITION, float2 Texture : TEXCOORD )
{
    float4x4 reflectProjection;
    pos.w = 1.0f;
    float4 WorldPos = mul( pos, World );
    VSOut output = (VSOut)0;
    output.Pos = mul( pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = Texture;
    reflectProjection = mul( ReflectView, Projection );
    reflectProjection = mul( World, reflectProjection );
    output.Reflection = mul( pos, reflectProjection );

    return output;
}
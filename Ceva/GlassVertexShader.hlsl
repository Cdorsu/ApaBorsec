
struct VSOut
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD;
    float4 refractPos : POSITION0;
};

cbuffer cbPerObject : register( b0 )
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};

VSOut main( float4 pos : POSITION, float2 tex : TEXCOORD )
{
    float4x4 viewProjWorld;
    VSOut output;
    pos.w = 1.0f;
    output.pos = mul( pos, World );
    output.pos = mul( output.pos, View );
    output.pos = mul( output.pos, Projection );

    output.tex = tex;

    viewProjWorld = mul( View, Projection );
    viewProjWorld = mul( World, viewProjWorld );
    output.refractPos = mul( pos, viewProjWorld );

    return output;
}
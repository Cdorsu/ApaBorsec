
struct VSOut
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD;
    float3 Nor : NORMAL;
    float3 ViewDirection : POSITION;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

cbuffer cbPerObject
{
    float4x4 WVP;
    float4x4 World;
    float3 CamPos;
    float pad;
};

VSOut main( float4 pos : POSITION, float2 Texture : TEXCOORD, float3 Normal : NORMAL, float3 Binormal : BINORMAL, float3 Tangent : TANGENT )
{
    float4 WorldPos = mul( pos, World );
    VSOut output = (VSOut)0;
    output.Pos = mul( pos, WVP );
    output.Tex = Texture;
    output.Nor = mul( Normal, (float3x3)World );
    output.Nor = normalize( output.Nor );
    output.Tangent = mul( Tangent, (float3x3)World );
    output.Tangent = normalize( output.Tangent );
    output.Binormal = mul( Binormal, (float3x3)World );
    output.Binormal = normalize( output.Binormal );
    output.ViewDirection = CamPos.xyz - WorldPos.xyz;
    output.ViewDirection = normalize( output.ViewDirection );
    return output;
}
struct VSOut
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD;
    float3 Nor : NORMAL;
    float3 ViewDirection : POSITION;
    float fogFactor : FOG;
};

cbuffer cbPerObject : register ( b0 )
{
    float4x4 WVP;
    float4x4 World;
    float3 CamPos;
    float pad;
};

cbuffer cbFog : register ( b1 )
{
    float fogStart;
    float fogEnd;
};

VSOut main( float4 pos : POSITION, float2 Texture : TEXCOORD, float3 Normal : NORMAL, float3 Binormal : BINORMAL, float3 Tangent : TANGENT )
{
    float4 WorldPos = mul( pos, World );
    VSOut output = (VSOut)0;
    output.Pos = mul( pos, WVP );
    output.Tex = Texture;
    output.Nor = mul( Normal, (float3x3)World );
    output.Nor = normalize( output.Nor );
    output.ViewDirection = CamPos.xyz - WorldPos.xyz;
    output.ViewDirection = normalize( output.ViewDirection );
    output.fogFactor = saturate( 1.0f / pow( 2.71f, CamPos.z * 1.0f ) );
    return output;
}
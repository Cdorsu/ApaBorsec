struct VSOUT
{
    float4 Position : SV_Position;
    float2 Texture : TEXCOORD;
};

cbuffer cbPerObject : register ( b0 )
{
    float4x4 WVP;
};

VSOUT main( float4 pos : POSITION, float2 tex : TEXCOORD )
{
    VSOUT output;
    output.Position = mul( pos, WVP );
    output.Texture = tex;
    return output;
}
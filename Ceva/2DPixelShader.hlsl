Texture2D ObjTexture : register ( t0 );
SamplerState Sampler : register ( s0 );

cbuffer cbPerObject : register( b0 )
{
    float4 Color;
};

struct PSIN
{
    float4 Position : SV_Position;
    float2 Texture : TEXCOORD;
};

float4 main( PSIN input ) : SV_TARGET
{
    float4 color = ObjTexture.Sample( Sampler, input.Texture ) * Color;
    if ( color.r <= 0.0f )
        color.a = 0.0f;
    return color;
}
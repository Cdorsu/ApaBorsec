
Texture2D ObjTexture : register( t0 );
Texture2D ObjGlow : register( t1 );

SamplerState Sampler : register( s0 );

cbuffer cbGlow : register( b0 )
{
    float glowStrength;
};

struct PSIn
{
    float4 Position : SV_Position;
    float2 Texture : TEXCOORD;
};


float4 main ( PSIn input ) : SV_TARGET
{
    float4 textureColor = ObjTexture.Sample ( Sampler, input.Texture );
    float4 glowColor = ObjGlow.Sample ( Sampler, input.Texture );

    return glowColor * glowStrength + textureColor;
}
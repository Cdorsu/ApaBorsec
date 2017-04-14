
Texture2D ObjTexture : register( t0 );
Texture2D ObjMask : register( t1 );

SamplerState WrapSampler : register( s0 );

struct PSIn
{
    float4 Position : SV_Position;
    float2 Texture : TEXCOORD;
};


float4 main ( PSIn input ) : SV_TARGET
{
    float4 textureColor = ObjTexture.Sample ( WrapSampler, input.Texture );
    float4 maskColor = ObjMask.Sample ( WrapSampler, input.Texture );

    /*if ( maskColor.r == 0 && maskColor.g == 0 && maskColor.b == 0 )
        return float4 ( 1.0f, 1.0f, 0.0f, 1.0f );
    return textureColor;*/
    return textureColor * maskColor;
}
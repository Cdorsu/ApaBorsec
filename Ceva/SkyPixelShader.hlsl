

TextureCube Texture : register( t0 );
SamplerState WrapSampler : register( s0 );

struct PSIn
{
    float4 Position : SV_POSITION;
    float3 TexCoords : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
    return Texture.Sample(WrapSampler, input.TexCoords);
}
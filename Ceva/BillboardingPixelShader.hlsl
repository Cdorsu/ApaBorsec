
Texture2D ObjTexture : register( t0 );
SamplerState Sampler : register( s0 );

struct PSIn
{
    float4 position : SV_Position;
    float4 worldPos : POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
    return float4( 1.0f, 1.0f, 1.0f, 1.0f );
    float4 tex = ObjTexture.Sample( Sampler, input.texCoord );
    clip( tex.a - 0.25f );
    return tex;
}
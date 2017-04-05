
Texture2D ObjTexture : register( t0 );
Texture2D ObjProjection : register( t1 );

SamplerState WrapSampler : register( s0 );

struct PSIn
{
    float4 Position : SV_Position;
    float4 View : POSITION0;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

float4 main ( PSIn input ) : SV_TARGET
{
    float4 color = float4 ( 1.0f, 1.0f, 1.0f, 1.0f );
    float2 projection;

    projection.x = input.View.x / input.View.w / 2.0f + 0.5f;
    projection.y = -input.View.y / input.View.w / 2.0f + 0.5f;

    if ( projection.x == saturate ( projection.x ) && projection.y == saturate ( projection.y ) )
    {
        color = ObjProjection.Sample ( WrapSampler, projection );
    }
    else
    {
        color = ObjTexture.Sample ( WrapSampler, input.TexCoord );
    }
    return color;
}
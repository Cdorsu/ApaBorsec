
Texture2D textures : register ( t0 );
Texture2D textures1 : register ( t1 );
SamplerState Sampler : register ( s0 );

struct PSIn
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD;
    float4 Reflection : TEXCOORD1;
};

float4 main(PSIn input) : SV_TARGET
{
    float4 modelTexture = textures.Sample( Sampler, input.Tex );
    float2 reflectTexCoord;

    reflectTexCoord.x = input.Reflection.x / input.Reflection.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.Reflection.y / input.Reflection.w / 2.0f + 0.5f;

    float4 reflectionColor = textures1.Sample( Sampler, reflectTexCoord );

    //return reflectionColor;

    return lerp( modelTexture, reflectionColor, 0.25f );
}
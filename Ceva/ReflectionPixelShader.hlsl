
Texture2D textures[2];
SamplerState Sampler;

struct PSIn
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD;
    float4 Reflection : TEXCOORD1;
};

float4 main(PSIn input) : SV_TARGET
{
    float4 modelTexture = textures[0].Sample( Sampler, input.Tex );
    float2 reflectTexCoord;

    reflectTexCoord.x = input.Reflection.x / input.Reflection.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.Reflection.y / input.Reflection.w / 2.0f + 0.5f;

    float4 reflectionColor = textures[1].Sample( Sampler, reflectTexCoord );

    //return reflectionColor;

    return lerp( modelTexture, reflectionColor, 0.5f );
}
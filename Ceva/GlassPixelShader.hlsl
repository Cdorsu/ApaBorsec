
Texture2D ObjTexture : register( t0 );
Texture2D ObjBumpMap : register( t1 );
Texture2D ObjRefraction : register( t2 );
SamplerState Sampler : register( s0 );

struct PSIn
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD;
    float4 refractPos : POSITION0;
};

cbuffer GlassBuffer
{
    float refractionScale;
};

float4 main( PSIn input ) : SV_TARGET
{
    float4 diffuse = ObjTexture.Sample( Sampler, input.tex );
    float4 normals = ObjBumpMap.Sample( Sampler, input.tex );
    float2 refractTexCoord;

    normals = 2.0f * normals - 1.0f;

    refractTexCoord.x = input.refractPos.x / input.refractPos.w / 2.0f + 0.5f;
    refractTexCoord.y = -input.refractPos.y / input.refractPos.w / 2.0f + 0.5f;

    refractTexCoord += refractionScale * normals.xy;

    float4 refraction = ObjRefraction.Sample( Sampler, refractTexCoord );

    return lerp( refraction, diffuse, 0.5f );
}
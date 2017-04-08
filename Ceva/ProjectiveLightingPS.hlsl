
Texture2D ObjTexture : register( t0 );
Texture2D ObjLightmap : register( t1 );

SamplerState WrapSampler : register( s0 );
SamplerState ClampSampler : register( s1 );

cbuffer cbLight : register(b0)
{
    float4 Diffuse;
    float4 Ambient;
}

struct PSIn
{
    float4 Position : SV_Position;
    float4 View : POSITION0;
    float3 WorldPos : POSITION1;
    float3 LightVector : POSITION2;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

float4 main ( PSIn input ) : SV_TARGET
{
    float2 projectedTexCoord;
    float4 textureColor = ObjTexture.Sample ( WrapSampler, input.TexCoord );
    float4 color = Ambient;

    projectedTexCoord.x = input.View.x / input.View.w / 2.0f + 0.5f;
    projectedTexCoord.y = -input.View.y / input.View.w / 2.0f + 0.5f;
    
    if ( projectedTexCoord.x == saturate ( projectedTexCoord.x ) &&
        projectedTexCoord.y == saturate ( projectedTexCoord.y ) )
    {
        float howMuchLight = dot ( input.Normal, input.LightVector );
        if ( howMuchLight > 0.0f )
        {
            float4 projectionColor = ObjLightmap.Sample ( ClampSampler, projectedTexCoord );
            color = saturate ( Ambient + Diffuse * projectionColor );
            return color * textureColor;
        }
    }

    return Ambient * textureColor;
}
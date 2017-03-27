
Texture2D ObjTexture : register( t0 );
Texture2D ObjDepthmap : register( t1 );

SamplerState WrapSampler : register( s0 );
SamplerState ClampSampler : register( s1 );

struct PSIn
{
    float4 Position : SV_Position;
    float4 WorldPos : POSITION0;
    float4 LightViewProjection : POSITION1;
    float3 lightPos : POSITION2;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

cbuffer cbLight : register( b0 )
{
    float4 Diffuse;
    float4 Ambient;
};

float4 main(PSIn input) : SV_Target
{
    float2 projectedTexCoord;
    float bias = 0.001f;

    projectedTexCoord.x = input.LightViewProjection.x / input.LightViewProjection.w / 2.0f + 0.5f;
    projectedTexCoord.y = -input.LightViewProjection.y / input.LightViewProjection.w / 2.0f + 0.5f;

    if ( projectedTexCoord.x == saturate ( projectedTexCoord.x ) && projectedTexCoord.y == saturate ( projectedTexCoord.y ) )
    {
        float depth = ObjDepthmap.Sample ( ClampSampler, projectedTexCoord ).r;
        float lightDepthValue = input.LightViewProjection.z / input.LightViewProjection.w;
        lightDepthValue -= bias;
        if (depth > lightDepthValue)
        {
            /*float howMuchLight = dot ( input.Normal, input.lightPos );
            if ( howMuchLight > 0.0f )
            {
                return ObjTexture.Sample ( WrapSampler, input.TexCoord ) * Diffuse * howMuchLight;
            }*/
            return ObjTexture.Sample ( WrapSampler, input.TexCoord );
        }
        
    }
    return ObjTexture.Sample ( WrapSampler, input.TexCoord ) * Ambient;
}
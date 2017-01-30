SamplerState Sampler;
Texture2D ObjTexture[3];

struct PSIn
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD;
    float3 Nor : NORMAL;
    float3 ViewDirection : POSITION;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
    float fogFactor : FOG;
};

cbuffer cbLight // per frame
{
    float4 AmbientColor;
    float4 DiffuseColor;
    float4 SpecularColor;
    float3 LightDir;
    float SpecularPower;
}

cbuffer cbFog // per frame
{
    float4 fogColor;
};

float4 main( PSIn input ) : SV_TARGET
{

    float4 Color = AmbientColor;
    float4 TextureColor = ObjTexture[0].Sample( Sampler, input.Tex );
    float4 BumpColor = ObjTexture[1].Sample( Sampler, input.Tex );
    float4 SpecularIntensity = ObjTexture[2].Sample( Sampler, input.Tex );
    BumpColor = ( BumpColor * 2.0f ) - 1.0f;
    float3 LightDirection = -LightDir;

    float3 BumpNormal = ( input.Tangent * BumpColor.x ) + ( input.Binormal * BumpColor.y ) + ( input.Nor * BumpColor.z );
    BumpNormal = normalize( BumpNormal );
    float BumpLight = saturate( dot( BumpNormal, LightDirection ) );
    TextureColor = BumpLight * TextureColor;

    float4 Specular = float4( 0.0f, 0.0f, 0.0f, 0.0f );
    float3 Reflection;

    float LightIntensity = saturate( dot( input.Nor, LightDirection ) );
    if ( LightIntensity > 0.0f )
    {
        Color += LightIntensity * DiffuseColor;
        Reflection = normalize( 2 * LightIntensity * input.Nor - LightDirection );
        Specular = pow( saturate( dot( Reflection, input.ViewDirection ) ), SpecularPower );
        Specular *= SpecularIntensity;
        Specular *= SpecularColor;
        saturate( Specular );
        // Specular *= 0.2f; /// If specular is too high, use this
    }
    Color = Color * TextureColor;
    Color = saturate( Color + Specular );
    Color = input.fogFactor * Color + ( 1.0f - input.fogFactor ) * fogColor;
    //TextureColor = lerp( TextureColor, fogColor, input.fogFactor ); // In special cases, this looks better
    return Color;
}


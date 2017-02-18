
cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP; // Only projection
};

cbuffer cbWindowInfo : register( b1 )
{
    float windowHeight;
};

struct VSOut
{
    float4 Position : SV_Position;
    float2 tex1 : TEXCOORD1;
    float2 tex2 : TEXCOORD2;
    float2 tex3 : TEXCOORD3;
    float2 tex4 : TEXCOORD4;
    float2 tex5 : TEXCOORD5;
    float2 tex6 : TEXCOORD6;
    float2 tex7 : TEXCOORD7;
};

VSOut main ( float4 pos : POSITION, float2 tex : TEXCOORD )
{
    VSOut output = ( VSOut ) 0;
    output.Position = mul ( pos, WVP );
    
    float Texel = 1.0f / windowHeight;

    output.tex1 = tex + float2 ( 0.0f, -3 * Texel );
    output.tex2 = tex + float2 ( 0.0f, -2 * Texel );
    output.tex3 = tex + float2 ( 0.0f, -1 * Texel );
    output.tex4 = tex + float2 ( 0.0f, 0.0f );
    output.tex5 = tex + float2 ( 0.0f, 1 * Texel );
    output.tex6 = tex + float2 ( 0.0f, 2 * Texel );
    output.tex7 = tex + float2 ( 0.0f, 3 * Texel );

    return output;
}

cbuffer cbMatrices : register( b0 )
{
    float4x4 WVP;
    float toAdd;
};

struct VSOut
{
    float4 Position : SV_Position;
    float2 Texture : TEXCOORD;
    float2 RTexture : TEXCOORD1;
};

VSOut main ( float4 pos : POSITION, float2 inTex : TEXCOORD, float2 axis : TEXCOORD1 )
{
    VSOut output = ( VSOut ) 0;
    output.Position = mul ( pos, WVP );
    output.Texture = inTex;
    output.RTexture = inTex + axis * toAdd;
    return output;
}
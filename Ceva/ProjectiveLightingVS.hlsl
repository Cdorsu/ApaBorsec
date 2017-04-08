
cbuffer cbMatrices : register( b0 )
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;

    float4x4 View2;
    float4x4 Projection2;
}

cbuffer cbInfo : register( b1 )
{
    float3 LightPos;
}

struct VSOut
{
    float4 Position : SV_Position;
    float4 View : POSITION0;
    float3 WorldPos : POSITION1;
    float3 LightVector : POSITION2;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VSOut main ( float4 inPos : POSITION, float2 inTex : TEXCOORD, float3 inNor : NORMAL )
{
    VSOut output = ( VSOut ) 0;
    inPos.w = 1.0f;

    output.WorldPos = mul ( inPos, World ).xyz;

    output.Position = mul ( inPos, World );
    output.Position = mul ( output.Position, View );
    output.Position = mul ( output.Position, Projection );

    output.View = mul ( inPos, World );
    output.View = mul ( output.View, View2 );
    output.View = mul ( output.View, Projection2 );

    output.Normal = mul ( inNor, ( float3x3 ) World );
    output.Normal = normalize ( inNor );

    output.LightVector = LightPos.xyz - output.WorldPos.xyz;
    output.LightVector = normalize ( output.LightVector );

    output.TexCoord = inTex;

    return output;
}
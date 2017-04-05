
struct VSOut
{
    float4 Position : SV_Position;
    float4 View : POSITION0;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

cbuffer cbMatrices : register( b0 )
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;

    float4x4 View2;
    float4x4 Projection2;
}

VSOut main ( float4 inPos : POSITION, float3 inNor : NORMAL, float2 inTex : TEXCOORD )
{
    VSOut output = ( VSOut ) 0;
    inPos.w = 1.0f;

    output.Position = mul ( inPos, World );
    output.Position = mul ( output.Position, View );
    output.Position = mul ( output.Position, Projection );

    output.View = mul ( inPos, World );
    output.View = mul ( output.View, View2 );
    output.View = mul ( output.View, Projection2 );

    output.Normal = mul ( inPos, World );
    output.Normal = normalize ( output.Normal );

    output.TexCoord = inTex;

    return output;
}
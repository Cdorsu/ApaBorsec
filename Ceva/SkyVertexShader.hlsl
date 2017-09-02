
cbuffer cbPerObject : register( b0 )
{
	float4x4 World;
    float4x4 View;
    float4x4 Projection;
}

struct VSOut
{
	float4 Position : SV_POSITION;
	float3 TexCoords : TEXCOORD;
};

VSOut main( float4 pos : POSITION )
{
	VSOut Output = ( VSOut ) 0;

	Output.Position = mul( pos, World );
    Output.Position = mul( Output.Position, View );
    Output.Position = mul( Output.Position, Projection ).xyww;
	Output.TexCoords = pos.xyz;

	return Output;
}
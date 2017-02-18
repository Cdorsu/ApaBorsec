
struct VSOut
{
	float4 position : SV_Position;
	float4 depthPosition : POSITION0;
};

cbuffer cbPerObject
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

void main( in float4 pos : POSITION, out VSOut output )
{
	pos.w = 1;
	output.position = mul ( pos, World );
	output.position = mul ( output.position, View );
	output.position = mul ( output.position, Projection );

	output.depthPosition = output.position;
}
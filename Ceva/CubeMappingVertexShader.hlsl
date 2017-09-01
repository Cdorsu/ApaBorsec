
cbuffer cbPerObject : register(b0)
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

struct VSOut
{
	float4 Position : SV_POSITION;
	float3 TexCoords : TEXCOORD;
    float3 Normal : NORMAL;
};

VSOut main(float4 pos : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL)
{
	VSOut output = (VSOut) 0;
	output.Position = mul(pos, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	output.TexCoords = pos.xyz;
    output.Normal = mul(float4(normal, 1.0f), World).xyz;
	return output;
}
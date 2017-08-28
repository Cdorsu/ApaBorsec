
cbuffer cbPerObject : register(b0)
{
    float4x4 VP;
};

struct VSOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

VSOut main(float4 Position : POSITION, row_major float4x4 world : WORLD, float4 Color : COLOR)
{
	VSOut output = (VSOut) 0;
    float4x4 WVP = mul(world, VP);
    output.Position = mul(Position, WVP);
    output.Color = Color;
    return output;
}
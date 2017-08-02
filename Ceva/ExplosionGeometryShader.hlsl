
cbuffer cbPerObject
{
    float4x4 WVP;
    float4x4 World;
};

cbuffer cbTimeBuffer
{
    float t;
};

struct GSIn
{
    float4 Position : SV_Position;
    float2 Texture : TEXCOORD;
    float3 Normal : NORMAL;
};

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 Texture : TEXCOORD;
};

[maxvertexcount(3)]
void main(
	triangle GSIn input[3], 
	inout TriangleStream< GSOutput > output
)
{
    float3 AvgNormal = input[0].Normal + input[1].Normal + input[2].Normal;
    AvgNormal /= 3;
    float4 ANormal = float4(AvgNormal, 1.0f);
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
        element.pos = input[i].Position + mul(ANormal, World) * t;
        element.pos = mul(element.pos, WVP);
        element.Texture = input[i].Texture;
		output.Append(element);
	}
}
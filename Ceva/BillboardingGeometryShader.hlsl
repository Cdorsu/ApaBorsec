
cbuffer cbCamera : register( b0 )
{
    float3 camPos;
};

struct GSIn
{
    float4 position : SV_Position;
    float4 worldPos : POSITION;
    float4x4 world : TEXCOORD;
    float4x4 WVP : POSITION1;
};

struct GSOutput
{
    float4 position : SV_Position;
    float4 worldPos : POSITION;
    float2 texCoord : TEXCOORD;
};

[maxvertexcount(3)]
void main(
	triangle GSIn input[3],
	inout TriangleStream< GSOutput > outputStream
)
{
    for ( int i = 0; i < 3; ++i )
    {
        GSOutput output = (GSOutput)0;
        output.position = input[i].position;
        outputStream.Append( output );
    }

}
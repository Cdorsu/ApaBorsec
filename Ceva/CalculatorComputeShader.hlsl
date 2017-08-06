

struct Data
{
    float4 Pos;
    float2 Tex;
    float2 Pad;
};

StructuredBuffer<Data> Input1;
StructuredBuffer<Data> Input2;
RWStructuredBuffer<Data> Output;

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Output[DTid.x].Pos = Input1[DTid.x].Pos + Input2[DTid.x].Pos;
    Output[DTid.x].Tex = Input1[DTid.x].Tex + Input2[DTid.x].Tex;
}
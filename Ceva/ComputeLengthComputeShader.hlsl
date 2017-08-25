

StructuredBuffer<float3> Input : register(b0);
RWStructuredBuffer<float> Output : register(b1);


[numthreads(69, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 vec = Input[DTid.x];
    float length = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    length = sqrt(length);
    Output[DTid.x] = length;
}
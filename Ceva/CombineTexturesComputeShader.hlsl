
Texture2D T1;
Texture2D T2;
RWTexture2D<float4> Result;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	Result[DTid.xy] = T1[DTid.xy] +T2[DTid.xy];
}
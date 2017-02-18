
struct PSIn
{
	float4 Position : SV_Position;
	float4 depthValue : POSITION0;
};

float4 main ( in PSIn input ) : SV_Target
{
	float depth = input.depthValue.z / input.depthValue.w;
	return float4 ( depth, depth, depth, depth );
}
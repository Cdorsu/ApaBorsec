

#define GROUPSIZE 256
#define CACHESIZE GROUPSIZE + 2 * gRadius
#define MAXRADIUS 11

Texture2D gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

groupshared float4 gCache[256 + MAXRADIUS];


cbuffer blurInfo : register(b0)
{
	int gRadius;
	float weights[11];
};

static float gWeights[11] =
{
	0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
};

static int gBlurRadius = 5;

[numthreads(GROUPSIZE, 1, 1)]
void main(int3 dispatchThreadID : SV_DispatchThreadID,
			int3 groupThreadID : SV_GroupThreadID)
{
	int width, height;
	gInput.GetDimensions(width, height);
	if (groupThreadID.x < gBlurRadius)
	{
		int x = max(dispatchThreadID.x - gBlurRadius, 0);
		gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	else if (groupThreadID.x >= GROUPSIZE - gBlurRadius)
	{
		int x = min(width - 1, dispatchThreadID.x + gBlurRadius);
		gCache[groupThreadID.x + 2 * gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];
	}
	
	gCache[groupThreadID.x + gBlurRadius] = gInput[min(dispatchThreadID.xy, int2(width, height))];
	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);
	
	[loop]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + i + gBlurRadius;
		blurColor += gCache[k] * gWeights[i + gBlurRadius];
	}
	gOutput[dispatchThreadID.xy] = blurColor;

}
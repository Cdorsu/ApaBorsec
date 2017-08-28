

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

[numthreads(1, GROUPSIZE, 1)]
void main(int3 DTid : SV_DispatchThreadID,
			int3 GTid : SV_GroupThreadID)
{
	uint width, height;
	gInput.GetDimensions(width, height);
	if (GTid.y < gBlurRadius)
	{
		int y = max(DTid.y - gBlurRadius, 0);
		gCache[GTid.y] = gInput[int2(DTid.x, y)];
	}
	else if (GTid.y >= GROUPSIZE - gBlurRadius)
	{
		int y = min(height - 1, DTid.y + gBlurRadius);
		gCache[GTid.y + 2 * gBlurRadius] = gInput[int2(DTid.x, y)];
	}
	//gCache[GTid.y + gBlurRadius] = gInput[min(DTid.xy, int2(height, width) - 1)];
	gCache[GTid.y + gBlurRadius] = gInput[min(DTid.xy, gInput.Length.xy - 1)];
	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	[loop]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = gBlurRadius + i + GTid.y;
		blurColor += gCache[k] * gWeights[i + gBlurRadius];
	}

	gOutput[DTid.xy] = blurColor;
}
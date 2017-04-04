
Texture2D ObjDepthmap : register( t1 );

SamplerState ClampSampler : register( s1 );

struct PSIn
{
	float4 Position : SV_Position;
	float4 WorldPos : POSITION0;
	float4 LightViewProjection : POSITION1;
	float3 lightPos : POSITION2;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

float4 main ( PSIn input ) : SV_TARGET
{
	float bias = 0.001f;
	float2 projectedLightView;

	projectedLightView.x = input.LightViewProjection.x / input.LightViewProjection.w / 2.0f + 0.5f;
	projectedLightView.y = -input.LightViewProjection.y / input.LightViewProjection.w / 2.0f + 0.5f;

	if ( projectedLightView.x == saturate ( projectedLightView.x ) && projectedLightView.y == saturate ( projectedLightView.y ) )
	{
		float Depth = ObjDepthmap.Sample ( ClampSampler, projectedLightView ).r;
		float currentDepth = input.LightViewProjection.z / input.LightViewProjection.w;
		currentDepth -= bias;
		if ( Depth > currentDepth )
		{
			float howMuchLight = dot ( input.lightPos, input.Normal );
			if ( howMuchLight > 0.0f )
				return float4 ( 1.0f, 1.0f, 1.0f, 1.0f );
		}

	}


	return float4 ( 0.0f, 0.0f, 0.0f, 0.0f );
}
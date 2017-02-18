SamplerState Sampler : register ( s0 );
Texture2D ObjTexture : register ( t0 );

struct PSIn
{
	float4 Pos : SV_Position;
	float3 worldPos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float3 lightPos : TEXCOORD1;
};

struct DirectionalLight
{
	float4 Diffuse;
	float4 Ambient;
	float3 Direction;
};

struct PointLight
{
	float3 Position;
	float pad;
	float4 Diffuse;
	float3 Attenuation;
	float Range;
};

cbuffer cbSun : register ( b0 )
{
	DirectionalLight Sun;
};

cbuffer cbPointLights : register ( b1 )
{
	PointLight Point;
};

float4 main( PSIn input ) : SV_TARGET
{
	float4 textureColor = ObjTexture.Sample( Sampler,input.Tex );
	clip ( textureColor.a - 0.25f );
	float4 color = Sun.Ambient;
	float3 lightDir = -Sun.Direction;
	float4 color1 = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	float howMuchLight = dot( input.Normal, lightDir );
	if ( howMuchLight > 0.0f )
	{
		color += ( Sun.Diffuse * howMuchLight );
	}

	color = saturate( color );

	float3 finalColor = float3( 0.0f, 0.0f, 0.0f );
	float3 finalAmbient = textureColor.xyz * Sun.Ambient.xyz;

	float3 lightToPixelVec = Point.Position - input.worldPos;
	float size = length( lightToPixelVec );
	if ( size > Point.Range )
		return textureColor * color;

	lightToPixelVec /= size;
	howMuchLight = dot( lightToPixelVec, input.Normal );
	if ( howMuchLight > 0.0f )
	{
		finalColor += howMuchLight * Point.Diffuse.xyz * textureColor.xyz;
		finalColor /= Point.Attenuation[0] + ( size * Point.Attenuation[1] ) + ( ( size * size ) * Point.Attenuation[2] );
	}

	finalColor = saturate( finalAmbient + finalColor );
	return float4( finalColor, 1.0f );

}



Texture2D ObjTexture : register( t0 );
SamplerState Sampler : register( s0 );

struct PSIn
{
	float4 Position : SV_Position;
	float2 Tex : TEXCOORD;
	float2 tex1 : TEXCOORD1;
	float2 tex2 : TEXCOORD2;
	float2 tex3 : TEXCOORD3;
	float2 tex4 : TEXCOORD4;
	float2 tex5 : TEXCOORD5;
	float2 tex6 : TEXCOORD6;
	float2 tex7 : TEXCOORD7;
};


float4 main(PSIn input) : SV_TARGET
{
	float weight0, weight1, weight2, weight3;
	float normalization;
	float4 color = float4 ( 0.0f, 0.0f, 0.0f, 0.0f );

	weight0 = 1.0f;
	weight1 = 0.9f;
	weight2 = 0.645f;
	weight3 = 0.561f;

    normalization = ( weight0 + 2.0f * ( weight1 + weight2 + weight3 ) );

    weight0 /= normalization;
    weight1 /= normalization;
    weight2 /= normalization;
    weight3 /= normalization;


	color += ObjTexture.Sample ( Sampler, input.tex1 ) * weight3;
	color += ObjTexture.Sample ( Sampler, input.tex2 ) * weight2;
	color += ObjTexture.Sample ( Sampler, input.tex3 ) * weight1;
	color += ObjTexture.Sample ( Sampler, input.tex4 ) * weight0;
	color += ObjTexture.Sample ( Sampler, input.tex5 ) * weight1;
	color += ObjTexture.Sample ( Sampler, input.tex6 ) * weight2;
	color += ObjTexture.Sample ( Sampler, input.tex7 ) * weight3;

	color.a = 1.0f;

	return color;
}
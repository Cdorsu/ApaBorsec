struct GSOutput
{
	float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    uint ID : SV_PrimitiveID;
};

cbuffer WVP
{
    float4x4 WVP;
};

cbuffer cbPerFrame
{
    float3 camPos;
};

[maxvertexcount(4)]
void main(
	point float4 input[1] : SV_POSITION, 
	uint primID : SV_PrimitiveID,
	inout TriangleStream< GSOutput > output
)
{
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//

    float2 gTexC[4] =
    {
        float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
    };

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = camPos - input[0].xyz;
    look.y = 0.0f; // y-axis aligned, so project to xz-plane
    look = normalize(look);
    float3 right = cross(up, look);

	//
	// Compute triangle strip vertices (quad) in world space.
	//
    float halfWidth = 0.5f * 3;
    float halfHeight = 0.5f * 3;
	
    float4 v[4];
    v[0] = float4(input[0].xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].xyz - halfWidth * right + halfHeight * up, 1.0f);

	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
    GSOutput gout;
	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.pos = mul(v[i], WVP);
        gout.tex = gTexC[i];
        gout.ID = primID;
		
        output.Append(gout);
    }
}
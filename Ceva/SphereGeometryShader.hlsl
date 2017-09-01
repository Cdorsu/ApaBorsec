struct GSOutput
{
	float4 pos : SV_POSITION;
};

cbuffer cbPerObject
{
	float4x4 WVP;
	float3 EyePos;
};

void Subdivide(float4 inVerts[3], out float4 outVerts[6])
{
	//       1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// 0    m2     2

    float4 m[3];

	// Compute edge midpoints.
    m[0] = 0.5f * (inVerts[0] + inVerts[1]);
    m[1] = 0.5f * (inVerts[1] + inVerts[2]);
    m[2] = 0.5f * (inVerts[2] + inVerts[0]);
 
	// Project onto unit sphere
    m[0] = normalize(m[0]);
    m[1] = normalize(m[1]);
    m[2] = normalize(m[2]);

    outVerts[0] = inVerts[0];
    outVerts[1] = m[0];
    outVerts[2] = m[2];
    outVerts[3] = m[1];
    outVerts[4] = inVerts[2];
    outVerts[5] = inVerts[1];
};

[maxvertexcount(6)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > Stream
)
{
	float4 output[6];
    Subdivide(input, output);
    for (int i = 0; i < 5; ++i)
    {
        GSOutput vertex;
        vertex.pos = mul(output[i], WVP);
        Stream.Append(vertex);
    }
    GSOutput vertex;
    Stream.RestartStrip();
    vertex.pos = mul(output[1], WVP);
    Stream.Append(vertex);
    vertex.pos = mul(output[5], WVP);
    Stream.Append(vertex);
    vertex.pos = mul(output[3], WVP);
    Stream.Append(vertex);

}
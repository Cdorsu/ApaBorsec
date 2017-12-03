#define NUM_CONTROL_POINTS 2

struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[2]			: SV_TessFactor;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
};

cbuffer cbPoints : register(b1)
{
    uint NumControlPoints;
    float3 pad;
    float4 ControlPoints[100];
};


float Factorial(int numar)
{
    //if (numar < 33)
    //    return FactorialValues[numar];
    //return 263130836933693530167218012160000000.0f;
	
    float Produs = 1;
	[unroll(32)]
    for (int i = 2; i <  numar; ++i)
        Produs *= i;
    return Produs;
}

float Combinari(float N, float I)
{
    float nFac = Factorial(N);
    float iFac = Factorial(I);
    float inFac = Factorial(N - I);

    return nFac / (iFac * inFac);
}

float Bernstein(float N, float I, float T)
{
    float Basis;
    float ti;
    float tni;
    
    if (I == 0 && T == 0)
        ti = 1.0f;
	else
        ti = pow(abs(T), abs(I));

    if (N == I && T == 1)
        tni = 1.0f;
	else
        tni = pow(abs(1.0f - T), abs(N - I));

    Basis = Combinari(N, I) * ti * tni;
    return Basis;
}

[domain("isoline")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

    Output.vPosition = float4(
		patch[0].vPosition * domain + (1.0f - domain) * patch[1].vPosition, 1.0f
	);

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    [unroll(64)]
    for (uint i = 0; i < NumControlPoints; ++i)
    {
        float Basis = Bernstein(NumControlPoints - 1, i, domain);
        x += Basis * ControlPoints[i].x;
        y += Basis * ControlPoints[i].y;
        z += Basis * ControlPoints[i].z;
    }
    Output.vPosition = float4(x, y, z, 1.0f);

    Output.vPosition = mul(Output.vPosition, WVP);

    return Output;
}

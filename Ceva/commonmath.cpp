#include "commonmath.h"

#define u x
#define v y

namespace math
{
	using namespace DirectX;
#ifndef _USE_XMVECTOR_
	void CalculateTangentandBinormal( _In_ XMFLOAT3 v1, _In_ XMFLOAT3 v2, _In_ XMFLOAT3 v3,
		_In_ XMFLOAT2 uv1, _In_ XMFLOAT2 uv2, _In_ XMFLOAT2 uv3,
		_Out_ XMFLOAT3& Tangent, _Out_ XMFLOAT3& Binormal )
	{
		float length, den;
		XMFLOAT3 edge1, edge2;
		XMFLOAT2 edgeuv1, edgeuv2;

		// Calculate edges
		edge1.x = v2.x - v1.x;
		edge1.y = v2.y - v1.y;
		edge1.z = v2.z - v1.z;

		edge2.x = v3.x - v1.x;
		edge2.y = v3.y - v1.y;
		edge2.z = v3.z - v1.z;

		edgeuv1.u = uv2.u - uv1.u;
		edgeuv1.v = uv2.v - uv1.v;

		edgeuv2.u = uv3.u - uv1.u;
		edgeuv2.v = uv3.v - uv1.v;
		// Denominator for equations
		den = 1.0f / ( edgeuv1.u * edgeuv2.v - edgeuv1.v * edgeuv2.u );

		// Calculate tangent
		Tangent.x = ( edgeuv2.v * edge1.x - edgeuv1.v * edge2.x ) * den;
		Tangent.y = ( edgeuv2.v * edge1.y - edgeuv1.v * edge2.y ) * den;
		Tangent.z = ( edgeuv2.v * edge1.z - edgeuv1.v * edge2.z ) * den;

		// Normalize Tangent
		length = sqrtf( ( Tangent.x * Tangent.x ) + ( Tangent.y * Tangent.y ) + ( Tangent.z * Tangent.z ) );
		Tangent.x /= length;
		Tangent.y /= length;
		Tangent.z /= length;

		// Calculate binormal
		Binormal.x = ( edgeuv1.u * edge2.x - edgeuv2.u * edge1.x ) * den;
		Binormal.y = ( edgeuv1.u * edge2.y - edgeuv2.u * edge1.y ) * den;
		Binormal.z = ( edgeuv1.u * edge2.z - edgeuv2.u * edge1.z ) * den;

		// Normalize binormal
		length = sqrtf( ( Binormal.x * Binormal.x ) + ( Binormal.y * Binormal.y ) + ( Binormal.z * Binormal.z ) );
		Binormal.x /= length;
		Binormal.y /= length;
		Binormal.z /= length;


	}

	void CalculateNormal( _In_ XMFLOAT3 Tangent, _In_ XMFLOAT3 Binormal, _Out_ XMFLOAT3& Normal )
	{
		float length;
		// Calculate the normal
		Normal.x = (Tangent.y * Binormal.z) - (Tangent.z * Binormal.y);
		Normal.y = (Tangent.z * Binormal.x) - (Tangent.x * Binormal.z);
		Normal.z = (Tangent.x * Binormal.y) - (Tangent.y * Binormal.x);
		// Calculate the length
		length = sqrtf( (Normal.x * Normal.x) + (Normal.y * Normal.y) + (Normal.z * Normal.z) );
		// Normalize the normal
		Normal.x /= length;
		Normal.y /= length;
		Normal.z /= length;
	}
#else
#endif


}

#undef u
#undef v
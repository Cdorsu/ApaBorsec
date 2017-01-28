#pragma once

#include <Windows.h>
#include <DirectXMath.h>

namespace math
{
	using namespace DirectX;
#ifndef _USE_XMVECTOR_
	void CalculateTangentandBinormal( _In_ XMFLOAT3 v1, _In_ XMFLOAT3 v2, _In_ XMFLOAT3 v3,
		_In_ XMFLOAT2 uv1, _In_ XMFLOAT2 uv2, _In_ XMFLOAT2 uv3,
		_Out_ XMFLOAT3& Tangent, _Out_ XMFLOAT3& Binormal );
	void CalculateNormal( _In_ XMFLOAT3 Tangent, _In_ XMFLOAT3 Binormal, _Out_ XMFLOAT3& Normal );
#else
#endif
}


#pragma once

#include <vector>
#include <Windows.h>
#include <DirectXMath.h>

class CGeometryGenerator
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Tex;
		DirectX::XMFLOAT3 Nor;
	};
	struct MeshData
	{
		std::vector<SVertex> Vertices;
		std::vector<DWORD> indices;
	};
public:
	static void CreateGrid( float Width, float Depth, UINT N, UINT M, MeshData& mesh );
	static void CreateCylinder( float topRadius, float bottomRadius, float height, UINT stackCount, UINT slice, MeshData& mesh );
};


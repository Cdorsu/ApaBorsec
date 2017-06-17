#include "GeometryGenerator.h"


void CGeometryGenerator::CreateGrid( float Width, float Depth, UINT N, UINT M, MeshData& mesh )
{
	int vertexCount = N * M;
	int FaceCount = ( N - 1 ) * ( M - 1 ) * 2;

	float halfWidth = Width * 0.5f;
	float halfDepth = Depth * 0.5f;

	float cellSpacingWidth = Width / ( N - 1 );
	float cellSpacingDepth = Depth / ( M - 1 );

	float du = 1.0f / ( N - 1 );
	float dv = 1.0f / ( M - 1 );

	mesh.Vertices.resize( vertexCount );

	for ( UINT i = 0; i < M; ++i )
	{
		float depth = halfDepth - ( i * cellSpacingDepth );
		for ( UINT j = 0; j < N; ++j )
		{
			float width = -halfWidth + ( j * cellSpacingWidth );
			
			mesh.Vertices[ i * N + j ].Pos = DirectX::XMFLOAT3( width, 0.0f, depth );
			mesh.Vertices[ i * N + j ].Nor = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f );
			mesh.Vertices[ i * N + j ].Tex.x = du * j;
			mesh.Vertices[ i * N + j ].Tex.y = dv * i;

		}
	}

	mesh.indices.resize( FaceCount * 3 );
	UINT k = 0;
	for ( UINT i = 0; i < M - 1; i++ )
	{
		for ( UINT j = 0; j < N - 1; j++ )
		{
			mesh.indices[ k + 0 ] = i * N + j;
			mesh.indices[ k + 1 ] = i * N + j + 1;
			mesh.indices[ k + 2 ] = ( i + 1 ) * N + j + 1;
			mesh.indices[ k + 3 ] = i * N + j;
			mesh.indices[ k + 4 ] = ( i + 1 ) * N + j + 1;
			mesh.indices[ k + 5 ] = ( i + 1 ) * N + j;
				
			k += 6;
		}
	}
}
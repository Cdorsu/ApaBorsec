#pragma once

#include "common.h"
#include "commonmath.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <vector>

#include "Texture.h"


__declspec(align(16)) class CModel
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT3 Binormal;
		Vertex() { ZeroMemory( this, sizeof( Vertex ) ); };
		Vertex( float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz )
			:Position( x, y, z ), Texture( u, v ), Normal( nx, ny, nz ) {};
	};
private:
	ID3D11Buffer *VertexBuffer, *IndexBuffer;
	UINT VertexCount, IndexCount;
	DirectX::XMMATRIX World;
	CTexture *Texture;
	CTexture *BumpMap;
	CTexture *SpecularMap;
public:
	CModel();
	~CModel();
public:
	bool Initialize( ID3D11Device * device );
	bool Initialize( ID3D11Device * device, LPWSTR lpFilepath, LPWSTR Texture, LPWSTR Bumpmap, LPWSTR Specular );
	void Render( ID3D11DeviceContext * context );
	void Shutdown();
public:
	inline UINT GetVertexCount() { return VertexCount; };
	inline UINT GetIndexCount() { return IndexCount; };
	inline DirectX::XMMATRIX& GetWorld() { return World; };
	inline ID3D11ShaderResourceView* GetTexture() { return Texture->GetTexture(); };
	inline ID3D11ShaderResourceView* GetBumpMap() { return BumpMap->GetTexture(); };
	inline ID3D11ShaderResourceView* GetSpecularMap() { return SpecularMap->GetTexture(); };
	inline void Translate( float x, float y, float z ) { World *= DirectX::XMMatrixTranslation( x, y, z ); };
	inline void RotateX( float Angle ) { World *= DirectX::XMMatrixRotationX( Angle ); };
	inline void RotateY( float Angle ) { World *= DirectX::XMMatrixRotationY( Angle ); };
	inline void RotateZ( float Angle ) { World *= DirectX::XMMatrixRotationZ( Angle ); };
	inline void Identity() { World = DirectX::XMMatrixIdentity(); };
public:
	inline void* operator new(size_t size)
	{
		return _mm_malloc( size,16 );
	};
	inline void operator delete(void* object)
	{
		_mm_free( object );
	};
};


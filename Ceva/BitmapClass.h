#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <DirectXMath.h>

#include "Texture.h"

class BitmapClass
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
		Vertex( float x, float y, float z,
			float u, float v )
			:Position( x, y, z ), Texture( u, v ) {};
	};
private:
	ID3D11Buffer *VertexBuffer, *IndexBuffer;
	UINT VertexCount, IndexCount;
	CTexture *Texture;
	int ScreenWidth, ScreenHeight;
	int BitmapWidth, BitmapHeight;
	int PreviousX = -1, PreviousY = -1;
public:
	BitmapClass();
	~BitmapClass();
public:
	bool Initialize( ID3D11Device * device, LPWSTR lpFilepath, int iScreenWidth, int iScreenHeight, int iBitmapWidth, int iBitmapHeight );
	void Render( ID3D11DeviceContext * context, int PositionX, int PositionY );
	void Shutdown();
public:
	inline UINT GetVertexCount() { return VertexCount; };
	inline UINT GetIndexCount() { return IndexCount; };
	inline ID3D11ShaderResourceView* GetTexture() { return Texture->GetTexture(); };
private:
	void RenderBuffers( ID3D11DeviceContext* );
};


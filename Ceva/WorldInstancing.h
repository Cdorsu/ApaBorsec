#pragma once

#include <array>
#include "D3DClass.h"

class WorldInstancing
{
private:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		SVertex( float x, float y, float z ) :
			Position( x, y, z )
		{ };
	};
	struct SInstance
	{
		DirectX::XMFLOAT4X4 World;
		DirectX::XMFLOAT4 Color;
		SInstance( DirectX::FXMMATRIX& World, DirectX::XMFLOAT4 Color ) :
			Color( Color )
		{
			DirectX::XMStoreFloat4x4( &( this->World ), World );
		}
	};
	struct SConstantBuffer
	{
		DirectX::XMMATRIX VP;
	};

private:
	ID3D11Buffer * m_indexBuffer;
	ID3D11Buffer * m_vertexBuffer;
	ID3D11Buffer * m_instanceBuffer;
	ID3D11Buffer * m_constantBuffer;

	ID3D11VertexShader * m_vertexShader;
	ID3D11InputLayout * m_inputLayout;
	ID3D11PixelShader * m_pixelShader;

	UINT m_vertexCount;
	UINT m_indexCount;
	UINT m_instanceCount;

private: // Taken form an upper level
	ID3D11Device * m_device;
	ID3D11DeviceContext * m_immediateContext;

public:
	WorldInstancing( ID3D11Device * device, ID3D11DeviceContext * immediateContext );
	~WorldInstancing( );

public:
	void Render( DirectX::FXMMATRIX& view, DirectX::FXMMATRIX& projection );

private:
	void InitializeBuffers( );
	void InitializeShaders( );
	void UpdateBuffers( DirectX::FXMMATRIX&, DirectX::FXMMATRIX& );
	void Shutdown( );

};


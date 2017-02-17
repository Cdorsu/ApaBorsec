#include "BitmapClass.h"

BitmapClass::BitmapClass()
{
	ZeroMemory( this, sizeof( BitmapClass ) );
}

bool BitmapClass::Initialize( ID3D11Device * device, LPWSTR lpFilepath, int iScreenWidth, int iScreenHeight, int iBitmapWidth, int iBitmapHeight )
{
	PreviousX = -1;
	PreviousY = -1;
	if (lpFilepath != L"")
	{
		Texture = new CTexture();
		if (!Texture->Initialize( device, lpFilepath ))
			return false;
	}
	ScreenWidth = iScreenWidth;
	ScreenHeight = iScreenHeight;
	BitmapWidth = iBitmapWidth;
	BitmapHeight = iBitmapHeight;

	HRESULT hr;
	VertexCount = 4;
	DWORD Indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};
	IndexCount = sizeof( Indices ) / sizeof( Indices[0] );
	D3D11_BUFFER_DESC buffDesc = { 0 };
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( Vertex ) * VertexCount;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer( &buffDesc, NULL, &VertexBuffer );
	if (FAILED( hr ))
		return false;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	buffDesc.ByteWidth = sizeof( Indices );
	buffData.pSysMem = Indices;
	hr = device->CreateBuffer( &buffDesc, &buffData, &IndexBuffer );
	if (FAILED( hr ))
		return false;

	return true;
}

void BitmapClass::Render( ID3D11DeviceContext * context, int PositionX, int PositionY )
{
	if (PositionX == PreviousX && PositionY == PositionY)
	{
		RenderBuffers( context );
		return;
	}
	PreviousX = PositionX;
	PreviousY = PositionY;

	float left = (float)(this->ScreenWidth / 2 * -1) + PositionX;
	float right = left + BitmapWidth;
	float top = (float)(this->ScreenHeight / 2) - PositionY;
	float bottom = top - BitmapHeight;

	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map( VertexBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped );
	Vertex vertices[] =
	{
		Vertex( left, top, 0.0f, 0.0f, 0.0f ),
		Vertex( right, top, 0.0f, 1.0f, 0.0f ),
		Vertex( right, bottom, 0.0f, 1.0f, 1.0f ),
		Vertex( left, bottom, 0.0f, 0.0f, 1.0f ),
	};
	memcpy( mapped.pData, vertices, sizeof( vertices ) );

	context->Unmap( VertexBuffer, 0 );

	RenderBuffers( context );

}

void BitmapClass::RenderBuffers( ID3D11DeviceContext * context )
{
	UINT Stride = sizeof( Vertex );
	UINT Offset = 0;
	context->IASetVertexBuffers( 0, 1, &VertexBuffer, &Stride, &Offset );
	context->IASetIndexBuffer( IndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

void BitmapClass::Shutdown()
{
	Texture->Shutdown();
	delete Texture;
	SafeRelease( VertexBuffer );
	SafeRelease( IndexBuffer );
}

BitmapClass::~BitmapClass()
{
	Shutdown();
}

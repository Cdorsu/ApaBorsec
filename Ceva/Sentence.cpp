#include "Sentence.h"



CSentence::CSentence()
{
	ZeroMemory( this, sizeof( CSentence ) );
}

bool CSentence::Initialize( ID3D11Device * device, char * sentence, int iScreenWidth, int iScreenHeight, float x, float y )
{
	ScreenWidth = iScreenWidth;
	ScreenHeight = iScreenHeight;
	Sentence = sentence;
	maxLength = strlen( sentence );
	x = ( ( float ) ScreenWidth / 2 * -1 ) + x;
	y = ( ( float ) ScreenHeight / 2 ) - y;
	HRESULT hr;
	D3D11_BUFFER_DESC buffDesc = { 0 };
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	IndexCount = ( UINT ) strlen( sentence ) * 6;
	std::vector<DWORD> indices(IndexCount);
	for (UINT i = 0; i < IndexCount; ++i)
		indices[i] = i;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	buffDesc.ByteWidth = sizeof( DWORD ) * IndexCount;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffData.pSysMem = &indices[0];
	hr = device->CreateBuffer( &buffDesc, &buffData, &IndexBuffer );
	if (FAILED( hr ))
		return false;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( FontClass::SVertex ) * IndexCount;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer( &buffDesc, NULL, &VertexBuffer );
	if (FAILED( hr ))
		return false;
	return true;
}

bool CSentence::Update( ID3D11DeviceContext * context, char * sentence, size_t length, float x, float y )
{
	if (sentence == Sentence.c_str())
		return true;
	if ( x == -1 && y == -1 )
	{
		x = CurrentX;
		y = CurrentY;
	}
	else
	{
		x = ( ( float ) ScreenWidth / 2 * -1 ) + x;
		y = ( ( float ) ScreenHeight / 2 ) - y;
	}
	if ( length > maxLength )
		return false;
	Sentence = sentence;
	if ( length <= maxLength )
	{
		IndexCount = ( length - 1 ) * 6;
		VertexCount = ( length - 1 ) * 6;
	}
	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map( VertexBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped );
	
	FontClass::BuildVertexArray( ( void* ) mapped.pData, sentence, x, y );

	context->Unmap( VertexBuffer, 0 );
	return true;
}

void CSentence::Render( ID3D11DeviceContext * context )
{	
	RenderBuffers( context );
}

void CSentence::RenderBuffers( ID3D11DeviceContext * context )
{
	UINT Stride = sizeof( FontClass::SVertex );
	UINT Offset = 0;
	context->IASetVertexBuffers( 0, 1, &VertexBuffer, &Stride, &Offset );
	context->IASetIndexBuffer( IndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

void CSentence::Shutdown()
{
	SafeRelease( VertexBuffer );
	SafeRelease( IndexBuffer );
}

CSentence::~CSentence()
{
	Shutdown();
}

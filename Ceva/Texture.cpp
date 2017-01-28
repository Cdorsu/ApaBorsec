#include "Texture.h"



CTexture::CTexture()
{
	ZeroMemory( this, sizeof( CTexture ) );
}

bool CTexture::Initialize( ID3D11Device * device, LPWSTR filepath )
{
	HRESULT hr;

	hr = D3DX11CreateShaderResourceViewFromFile( device, filepath, NULL, NULL, &Texture, NULL );
	if (FAILED( hr ))
	{
		MessageBox( NULL, L"Couldn't get texture from file",
			L"Error", MB_ICONERROR | MB_OK );
		return false;
	}

	return true;
}

void CTexture::Shutdown()
{
	SafeRelease( Texture );
}

CTexture::~CTexture()
{
	Shutdown();
}

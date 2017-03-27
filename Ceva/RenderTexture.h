#pragma once

#include "common.h"

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>


__declspec( align( 16 ) ) class CRenderTexture
{
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11ShaderResourceView* ShaderResourceView;
	ID3D11DepthStencilView* DSView;
	ID3D11DepthStencilState* DSState;
	ID3D11Texture2D* Texture;
	D3D11_VIEWPORT ViewPort;
	DirectX::XMMATRIX Projection;
	DirectX::XMMATRIX Ortographic;
public:
	CRenderTexture();
	~CRenderTexture();
public:
	bool Initialize( ID3D11Device * device, UINT WindowWidth, UINT WindowHeight, float FOV, float ScreenNear, float ScreenDepth );
	void Shutdown();
public:
	void SetRenderTarget( ID3D11DeviceContext * context );
	void BeginScene( ID3D11DeviceContext * context, common::Color& ClearColor );
	ID3D11ShaderResourceView* GetTexture( );
public:
	DirectX::XMMATRIX& GetOrthoMatrix( ) { return Ortographic; };
	DirectX::XMMATRIX& GetProjection( ) { return Projection; };
public:
	void * operator new( size_t size )
	{
		return _mm_malloc( size, 16 );
	}
	void operator delete ( void* obj )
	{
		_mm_free( obj );
	}
};


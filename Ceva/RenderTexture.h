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
	inline void SetRenderTarget( ID3D11DeviceContext * context )
	{
		context->RSSetViewports( 1, &ViewPort );
		context->OMSetRenderTargets( 1, &RenderTargetView, DSView );
	};
	inline void BeginScene( ID3D11DeviceContext * context, common::Color color )
	{
		context->ClearRenderTargetView( RenderTargetView, color );
		context->ClearDepthStencilView( DSView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0 );
	}
	inline ID3D11ShaderResourceView* GetTexture()
	{
		return ShaderResourceView;
	};
	inline DirectX::XMMATRIX GetProjection( )
	{
		return Projection;
	};
	inline DirectX::XMMATRIX GetOrthoMatrix( )
	{
		return Ortographic;
	};
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


#pragma once

#define UNICODE

#include "common.h"

#include <Windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <DXGI.h>
#include <fstream>
#include <DirectXMath.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DX11.lib")
#pragma comment (lib, "d3d10.lib")
#pragma comment (lib, "D3DX10.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D3DCompiler.lib")

__declspec(align(16)) class D3DClass
{
	static common::Color BackgroundColor;
	IDXGISwapChain *m_SwapChain;
	ID3D11RenderTargetView *m_RenderTargetView;
	ID3D11DepthStencilView *m_DepthStencilView;
	ID3D11Device *m_d3d11Device;
	ID3D11DeviceContext *m_d3d11DeviceContext;
	ID3D11RasterizerState *NoCulling;
	ID3D11RasterizerState *Wireframe;
	ID3D11DepthStencilState *DSDefaultState;
	ID3D11DepthStencilState *DSLessEqual;
	ID3D11DepthStencilState *DSWriteStencil;
	ID3D11DepthStencilState *DSDrawReflection;
	ID3D11DepthStencilState *DSDrawShadow;
	ID3D11BlendState *AlphaBlendingEnabled;
	ID3D11BlendState *NoRenderTargetWrite;
	ID3D11BlendState *Transparency;
	ID3D11BlendState *ShadowRendering;
	D3D11_VIEWPORT DefaultViewPort;
	LPWSTR m_GPU;
	UINT m_VideoMemory;
	DirectX::XMMATRIX m_OrthoMatrix;
public:
	D3DClass();
	~D3DClass();
public:
	bool Initialize( HINSTANCE hInstance, HWND hWnd, UINT Width, UINT Height, FLOAT NearZ, FLOAT FarZ );
	void BeginScene();
	void EndScene();
	void Shutdown();
public:
	inline ID3D11Device* GetDevice() { return m_d3d11Device; };
	inline ID3D11DeviceContext* GetImmediateContext() { return m_d3d11DeviceContext; };
	inline void ResetViewPort( ) { m_d3d11DeviceContext->RSSetViewports( 1, &DefaultViewPort ); };
	inline void EnableBackFaceCulling() { m_d3d11DeviceContext->RSSetState( NULL ); };
	inline void DisableCulling() { m_d3d11DeviceContext->RSSetState( NoCulling ); };
	inline void EnableWireframe( ) { m_d3d11DeviceContext->RSSetState( Wireframe ); };
	inline void EnableSkyRendering( ) { m_d3d11DeviceContext->OMSetDepthStencilState( DSLessEqual, 0 ); };
	inline void DisableSkyRendering( ) { m_d3d11DeviceContext->OMSetDepthStencilState( 0, 0 ); };
	inline void DisableAlphaBlending() { m_d3d11DeviceContext->OMSetBlendState( NULL, NULL, 0xffffffff ); };
	inline void EnableTransparency( ) { m_d3d11DeviceContext->OMSetBlendState( Transparency, NULL, 0xffffffff ); }
	inline void EnableShadowRendering( ) 
	{
		D3DXCOLOR blendFactor = { 0.5f, 0.5f, 0.5f, 0.5f };
		m_d3d11DeviceContext->OMSetBlendState( ShadowRendering, blendFactor, 0xffffffff );
		m_d3d11DeviceContext->OMSetDepthStencilState( DSDrawShadow, 0 );
	};
	inline void EnableAlphaBlending()
	{
		D3DXCOLOR blendFactor = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_d3d11DeviceContext->OMSetBlendState( AlphaBlendingEnabled, blendFactor, 0xffffffff );
	};
	inline void DisableRenderTargetWrite( )
	{
		m_d3d11DeviceContext->OMSetBlendState( NoRenderTargetWrite, nullptr, 0xffffffff );
	}
	inline void WriteToStencil( )
	{
		m_d3d11DeviceContext->OMSetDepthStencilState( DSWriteStencil, 1 );
	}
	inline void EnableMirrorRendering( )
	{
		m_d3d11DeviceContext->OMSetDepthStencilState( DSDrawReflection, 1 );
	}
	inline void EnableBackBuffer()
	{
		m_d3d11DeviceContext->OMSetRenderTargets( 1, &m_RenderTargetView, m_DepthStencilView );
	}
	inline void EnableTriangleListRendering( )
	{
		m_d3d11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}
	inline void EnablePointListRendering( )
	{
		m_d3d11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	}
	inline void ClearStencil( )
	{
		m_d3d11DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 0.0f, 0 );
	}
	inline ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView; };
	inline DirectX::XMMATRIX& GetOrthoMatrix() { return m_OrthoMatrix; };
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
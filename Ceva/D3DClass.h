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
	ID3D11BlendState *AlphaBlendingEnabled;
	ID3D11DepthStencilState *DSDefaultState;
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
	inline void EnableBackFaceCulling() { m_d3d11DeviceContext->RSSetState( NULL ); };
	inline void DisableCulling() { m_d3d11DeviceContext->RSSetState( NoCulling ); };
	inline void DisableAlphaBlending() { m_d3d11DeviceContext->OMSetBlendState( NULL, NULL, 0xffffffff ); };
	inline void EnableAlphaBlending()
	{
		D3DXCOLOR blendFactor = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_d3d11DeviceContext->OMSetBlendState( AlphaBlendingEnabled, blendFactor, 0xffffffff );
	};
	inline void EnableBackBuffer()
	{
		m_d3d11DeviceContext->OMSetRenderTargets( 1, &m_RenderTargetView, m_DepthStencilView );
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
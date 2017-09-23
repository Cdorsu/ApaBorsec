#pragma once

#include "D3DClass.h"
#include "Model.h"
#include "TextureShader.h"

__declspec( align( 16 ) ) class CubeMapping
{
	static constexpr const UINT CubemapTextureSize = 256;
private:
	struct SCBPerObject
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};
	struct SCBPerFrame
	{
		DirectX::XMFLOAT3 CamPos;
		float pad;
	};
public:
	CModel * m_model;
	CModel * m_sky;
	CModel * m_floor;

	ID3D11VertexShader * m_vertexShader;
	ID3D11InputLayout * m_inputLayout;
	ID3D11PixelShader * m_pixelShader;

	ID3D11VertexShader * m_SkyVertexShader;
	ID3D11InputLayout * m_SkyInputLayout;
	ID3D11PixelShader * m_SkyPixelShader;

	ID3D11Buffer * m_perObjectBuffer;
	ID3D11Buffer * m_perFrameBuffer;
	ID3D11SamplerState * m_sampler;

	ID3D11DepthStencilView * m_CubemapDSView;
	ID3D11RenderTargetView * m_DynamicRenderTargets[ 6 ];
	ID3D11ShaderResourceView * m_CubemapTexture;
	D3D11_VIEWPORT m_CubemapViewport;
	DirectX::XMMATRIX m_Views[ 6 ];
	DirectX::XMMATRIX m_Projection;

	ID3D11ShaderResourceView * m_textureSRV;

	ID3D11DepthStencilState * m_DSState;

public: // Do not delete these pointers
	ID3D11Device * m_device;
	ID3D11DeviceContext * m_immediateContext;
	CTextureShader * m_textureShader;


public:
	CubeMapping( CTextureShader * Shader, ID3D11Device * device, ID3D11DeviceContext * immediateContext );
	~CubeMapping( );

public:
	void Render( DirectX::XMFLOAT3 eyePos, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );

private:
	void CreateModels( );
	void CreateShaders( );
	void CreateBuffers( );
	void CreateRTVS( );
	void CreateMatrices( );

public:
	inline void* operator new ( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete ( void* object )
	{
		_aligned_free( object );
	}
};


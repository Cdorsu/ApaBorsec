#pragma once

#include "D3DClass.h"
#include "Model.h"

class CubeMapping
{
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

	ID3D11VertexShader * m_vertexShader;
	ID3D11InputLayout * m_inputLayout;
	ID3D11PixelShader * m_pixelShader;

	ID3D11VertexShader * m_SkyVertexShader;
	ID3D11InputLayout * m_SkyInputLayout;
	ID3D11PixelShader * m_SkyPixelShader;

	ID3D11Buffer * m_perObjectBuffer;
	ID3D11Buffer * m_perFrameBuffer;
	ID3D11SamplerState * m_sampler;

	ID3D11ShaderResourceView * m_textureSRV;

	ID3D11DepthStencilState * m_DSState;

	ID3D11Device * m_device;
	ID3D11DeviceContext * m_immediateContext;


public:
	CubeMapping( ID3D11Device * device, ID3D11DeviceContext * immediateContext );
	~CubeMapping( );

public:
	void Render( DirectX::XMFLOAT3 eyePos, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );

private:
	void CreateShaders( );
	void CreateBuffers( );
};


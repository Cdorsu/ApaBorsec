#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

#include "Light.h"
#include "PointLight.h"

class CBillboardShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX WVP;
		DirectX::XMMATRIX World;
	};
	struct SCamPos
	{
		DirectX::XMFLOAT3 camPos;
		float pad;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11GeometryShader *GeometryShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *CamBuffer;
	ID3D11SamplerState *Sampler;
public:
	CBillboardShader( );
	~CBillboardShader( );
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
		ID3D11ShaderResourceView * Texture,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
		DirectX::XMFLOAT3 camPos );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};


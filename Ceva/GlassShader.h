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

class CGlassShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};
	struct SGlass
	{
		float refractionScale;
		DirectX::XMFLOAT3 pad;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *GlassBuffer;
	ID3D11SamplerState *Sampler;
public:
	CGlassShader( );
	~CGlassShader( );
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
		ID3D11ShaderResourceView * Texture, ID3D11ShaderResourceView * Refraction,
		ID3D11ShaderResourceView * Bumpmap,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
		float refractionScale );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};


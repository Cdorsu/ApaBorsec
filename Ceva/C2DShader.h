#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

class C2DShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX WVP;
	};
	struct SColorBuffer
	{
		common::Color Color;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *ColorBuffer;
	ID3D11SamplerState *Sampler;
public:
	C2DShader();
	~C2DShader();
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount, ID3D11ShaderResourceView * ShaderResource,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection, common::Color Color = common::HexToRGB( 0xFFFFFF ) );
	void Shutdown();
private:
	void OutputShaderError( ID3D10Blob * Error );
};


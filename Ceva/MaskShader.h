#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

class CMaskShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX WVP;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11SamplerState *WrapSampler;
public:
	CMaskShader( );
	~CMaskShader( );
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount, ID3D11ShaderResourceView * texture,
		ID3D11ShaderResourceView * mask, DirectX::XMMATRIX& World, DirectX::XMMATRIX& View,
		DirectX::XMMATRIX& Projection );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};

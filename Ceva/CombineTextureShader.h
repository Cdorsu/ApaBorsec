#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

class CCombineTextureShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX WVP;
		float ToAdd;
		DirectX::XMFLOAT3 Pad;
	};
	struct SGlowInfo
	{
		float glowStrength;
		DirectX::XMFLOAT3 pad;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *GlowInfoBuffer;
	ID3D11SamplerState *WrapSampler;
public:
	CCombineTextureShader( );
	~CCombineTextureShader( );
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount, ID3D11ShaderResourceView * texture,
		ID3D11ShaderResourceView * glow, DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, 
		DirectX::XMMATRIX& Projection, float toAdd, float glowStrength );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};


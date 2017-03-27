#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

#include "LightView.h"

class CShadowShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX LightView;
		DirectX::XMMATRIX LightProjection;
	};
	struct SLightVS
	{
		DirectX::XMFLOAT3 Pos;
		float pad;
	};
	struct SLightPS
	{
		common::Color Diffuse;
		common::Color Ambient;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11SamplerState *WrapSampler;
	ID3D11SamplerState *ClampSampler;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *VSLightBuffer;
	ID3D11Buffer *PSLightBuffer;
	ID3D11InputLayout *InputLayout;
public:
	CShadowShader( );
	~CShadowShader( );
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
		ID3D11ShaderResourceView * Texture, ID3D11ShaderResourceView * Depthmap,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
		CLightView* Light );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};


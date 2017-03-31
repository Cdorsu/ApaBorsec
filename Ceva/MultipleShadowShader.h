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

class CMultipleShadowShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX LightView1;
		DirectX::XMMATRIX LightProjection1;
		DirectX::XMMATRIX LightView2;
		DirectX::XMMATRIX LightProjection2;
	};
	struct SLightVS
	{
		DirectX::XMFLOAT3 Pos1;
		float pad;
		DirectX::XMFLOAT3 Pos2;
		float pad2;
	};
	struct SLightPS
	{
		common::Color Diffuse1;
		common::Color Ambient1;
		common::Color Diffuse2;
		common::Color Ambient2;
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
	CMultipleShadowShader( );
	~CMultipleShadowShader( );
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
		ID3D11ShaderResourceView * Texture, ID3D11ShaderResourceView * Depthmap,
		ID3D11ShaderResourceView * Depthmap2,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
		CLightView* Light, CLightView* Light2 );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};


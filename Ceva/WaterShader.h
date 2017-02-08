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

class CWaterShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX Reflection;
	};
	struct SWater
	{
		float textureTranslation;
		float reflectionRefractionScale;
		DirectX::XMFLOAT2 pad;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *WaterBuffer;
	ID3D11SamplerState *Sampler;
public:
	CWaterShader( );
	~CWaterShader( );
public:
	bool Initialize( ID3D11Device * device );
	void Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
		ID3D11ShaderResourceView * ReflectionView, ID3D11ShaderResourceView * RefractionView,
		ID3D11ShaderResourceView * TextureView, DirectX::XMMATRIX& Reflection,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
		float textureTranslation, float reflectRefractScale );
	void Shutdown( );
private:
	void OutputShaderError( ID3D10Blob * Error );
};


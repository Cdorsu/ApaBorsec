#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

class CFogShader
{
private:
	struct SConstantBuffer
	{
		DirectX::XMMATRIX WVP;
		DirectX::XMMATRIX World;
		DirectX::XMFLOAT3 CamPos;
		float pad;
	};
	struct SLight
	{
		common::Color AmbientColor;
		common::Color DiffuseColor;
		common::Color SpecularColor;
		DirectX::XMFLOAT3 LightDir;
		float SpecularPower;
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *LightBuffer;
	ID3D11SamplerState *Sampler;
public:
	CFogShader();
	~CFogShader();
public:
	bool Initialize(ID3D11Device * device);
	void Render(ID3D11DeviceContext * context, UINT IndexDrawAmount,
		ID3D11ShaderResourceView * Texture, ID3D11ShaderResourceView * BumpMap, ID3D11ShaderResourceView* Specular,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
		DirectX::XMFLOAT3 CameraPos,
		common::Color AmbientColor, common::Color DiffuseColor, common::Color SpecularColor,
		DirectX::XMFLOAT3 LightDir, float SpecularPower);
	void Shutdown();
private:
	void OutputShaderError(ID3D10Blob * Error);
};


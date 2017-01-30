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
	struct SPSFog
	{
		common::Color FogColor;
	};
	struct SVSFog
	{
		float fogStart;
		float fogEnd;
		float pad1;
		float pad2;
	};
public:
	enum class EFogType
	{
		LinearFog,
		ExponentialFog,
		ExponentialFog2,
	};
private:
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader *PixelShader;
	ID3D11InputLayout *InputLayout;
	ID3D11Buffer *ConstantBuffer;
	ID3D11Buffer *LightBuffer;
	ID3D11Buffer *FogVSBuffer;
	ID3D11Buffer *FogPSBuffer;
	ID3D11SamplerState *Sampler;
public:
	CFogShader();
	~CFogShader();
public:
	bool Initialize(ID3D11Device * device, EFogType fog);
	void Render(ID3D11DeviceContext * context, UINT IndexDrawAmount,
		ID3D11ShaderResourceView * Texture, ID3D11ShaderResourceView * BumpMap, ID3D11ShaderResourceView * Specular,
		DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
		DirectX::XMFLOAT3 CameraPos, CLight* lightInfo, float fogStart, float fogEnd, common::Color fogColor);
	void Shutdown();
private:
	void OutputShaderError(ID3D10Blob * Error);
};


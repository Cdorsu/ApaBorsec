#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

class CComputeShader
{
private:
	ID3D11ComputeShader * ComputeShader;
public:
	CComputeShader();
	~CComputeShader();
public:
	bool Initialize(ID3D11Device * device);
	void Calculate(ID3D11DeviceContext * context, ID3D11ShaderResourceView * ShaderResource,
		ID3D11ShaderResourceView * ShaderResource2, ID3D11UnorderedAccessView * Result,
		UINT X, UINT Y, UINT Z);
	void Shutdown();
private:
	void OutputShaderError(ID3D10Blob * Error);
};


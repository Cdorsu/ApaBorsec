#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <fstream>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

class CVertexAdditionShader
{
public:
	struct Data
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 Texture;
		DirectX::XMFLOAT2 Pad;
		Data(DirectX::XMFLOAT4 Pos, DirectX::XMFLOAT2 Tex)
			: Position(Pos), Texture(Tex) {};
	};
private:
	ID3D11ComputeShader * ComputeShader;
public:
	CVertexAdditionShader();
	~CVertexAdditionShader();
public:
	bool Initialize(ID3D11Device * device);
	void Calculate(ID3D11DeviceContext * context, ID3D11ShaderResourceView * Input1,
		ID3D11ShaderResourceView * Input2, ID3D11UnorderedAccessView * Result, 
		ID3D11Buffer * OutputBuffer, ID3D11Buffer * DebugBuffer, UINT DebugSize,
		UINT X, UINT Y, UINT Z);
	bool CreateBuffers(ID3D11Device* device, Data * data1, Data * data2, size_t size,
		ID3D11Buffer ** InputBuffers, ID3D11Buffer *& OutputBuffer, ID3D11Buffer *& DebugBuffer,
		ID3D11ShaderResourceView ** InputSRVs, ID3D11UnorderedAccessView *& ResultUAV);
	void Shutdown();
private:
	void OutputShaderError(ID3D10Blob * Error);
};


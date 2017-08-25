#pragma once

#include "D3DClass.h"

__declspec(align(16)) class TesselatedWaves sealed
{
private:
	struct SPerObjectBuffer
	{
		DirectX::XMMATRIX WVP;
	};
	struct SPerObjectWorld
	{
		DirectX::XMMATRIX World;
	};
	struct SPerFrame
	{
		DirectX::XMFLOAT3 CamPos;
		float pad;
	};

private:
	ID3D11VertexShader * m_vertexShader;
	ID3D11HullShader * m_hullShader;
	ID3D11DomainShader * m_domainShader;
	ID3D11PixelShader * m_pixelShader;

	ID3D11InputLayout * m_layout;

	ID3D11Buffer * m_vertexBuffer;
	ID3D11Buffer * m_PerFrame;
	ID3D11Buffer * m_PerObject;
	ID3D11Buffer * m_PerObjectWorld;

	DirectX::XMMATRIX mWorld;

	ID3D11Device * m_device;
	ID3D11DeviceContext * m_immediateContext;

public:
	TesselatedWaves(ID3D11Device*, ID3D11DeviceContext*);
	~TesselatedWaves();
	
public:
	void Render(DirectX::XMFLOAT3 CamPos, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection);

private:
	void CreateShaders();
	void CreateBuffers();

public:
	inline void* operator new(size_t size)
	{
		return _aligned_malloc(size, 16);
	}
	inline void operator delete(void* object)
	{
		_aligned_free(object);
	}
};


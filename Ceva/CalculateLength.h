#pragma once

#include "common.h"
#include "D3DClass.h"

class CalculateLength
{
public:
	struct SData
	{
		DirectX::XMFLOAT3 Vector;
		SData(DirectX::XMFLOAT3 Data) :
			Vector(Data) { };
		SData(float xyz) :
			Vector(xyz, xyz, xyz) { };
		SData(float x, float yz) :
			Vector(x, yz, yz) { };
		SData(float x, float y, float z) :
			Vector(x, y, z) { };
	};

private:

	ID3D11Buffer * m_inputBuffer;
	ID3D11ShaderResourceView * m_inputSRV;

	ID3D11Buffer * m_outputBuffer;
	ID3D11UnorderedAccessView * m_outputUAV;

	ID3D11Buffer * m_debugBuffer;

	ID3D11Device * m_device;
	ID3D11DeviceContext * m_immediateContext;

	ID3D11ComputeShader * m_CalculateLengthShader;

	size_t m_size;

public:
	CalculateLength(ID3D11Device *, ID3D11DeviceContext *);
	~CalculateLength();

public:
	void SetData(SData *, size_t);
	void Calculate();
	void GetResults();
};


#include "ComputeShader.h"



CComputeShader::CComputeShader()
{
	ZeroMemory(this, sizeof(CComputeShader));
}

bool CComputeShader::Initialize(ID3D11Device * device)
{
	ID3D10Blob *ShaderBlob, *ErrorBlob;
	HRESULT hr;
	hr = D3DX11CompileFromFile(L"CombineTexturesComputeShader.hlsl", NULL, NULL, "main", "cs_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (ShaderBlob)
			OutputShaderError(ShaderBlob);
		return false;
	}
	hr = device->CreateComputeShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &ComputeShader);
	IFFAILED(hr, L"Couldn't create a compute shader");
	return true;
}

void CComputeShader::Calculate(ID3D11DeviceContext * context, ID3D11ShaderResourceView * ShaderResource,
	ID3D11ShaderResourceView * ShaderResource2, ID3D11UnorderedAccessView * Result, UINT X, UINT Y, UINT Z)
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	context->PSSetShaderResources(0, 1, nullSRV);
	context->PSSetShader(0, 0, 0);
	context->CSSetShaderResources(0, 1, &ShaderResource);
	context->CSSetShaderResources(1, 1, &ShaderResource2);
	context->CSSetUnorderedAccessViews(0, 1, &Result, nullptr);
	context->CSSetShader(ComputeShader, 0, 0);
	context->Dispatch(X, Y, Z);
	// Unbind the input textures from the CS for good housekeeping.
	context->CSSetShaderResources(0, 1, nullSRV);

	// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
	// and a resource cannot be both an output and input at the same time.
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable compute shader.
	context->CSSetShader(0, 0, 0);
}

void CComputeShader::OutputShaderError(ID3D10Blob * Error)
{
	std::ofstream ofs("ShaderError.txt");
	size_t Size = Error->GetBufferSize();
	char *error = (char*)Error->GetBufferPointer();
	for (size_t i = 0; i < Size; ++i)
		ofs << error[i];
	ofs.close();
}

void CComputeShader::Shutdown()
{
	SafeRelease(this->ComputeShader);
}

CComputeShader::~CComputeShader()
{
	Shutdown();
}

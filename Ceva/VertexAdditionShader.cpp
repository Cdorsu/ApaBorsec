#include "VertexAdditionShader.h"



CVertexAdditionShader::CVertexAdditionShader()
{
}

bool CVertexAdditionShader::Initialize(ID3D11Device * device)
{
	ID3D10Blob *ShaderBlob;
	HRESULT hr;
	/*hr = D3DX11CompileFromFile(L"CalculatorComputeShader.hlsl", NULL, NULL, "main", "cs_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (ShaderBlob)
			OutputShaderError(ShaderBlob);
		return false;
	}*/
	hr = D3DReadFileToBlob(L"CalculatorComputeShader.cso", &ShaderBlob);
	IFFAILED(hr, L"Fraier");
	hr = device->CreateComputeShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &ComputeShader);
	IFFAILED(hr, L"Couldn't create a compute shader");
	return true;
}

void CVertexAdditionShader::Calculate(ID3D11DeviceContext * context, ID3D11ShaderResourceView * Input1,
	ID3D11ShaderResourceView * Input2, ID3D11UnorderedAccessView * Result,
	ID3D11Buffer * OutputBuffer, ID3D11Buffer * DebugBuffer, UINT DebugSize,
	UINT X, UINT Y, UINT Z)
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	context->PSSetShaderResources(0, 1, nullSRV);
	context->PSSetShader(0, 0, 0);
	context->CSSetShaderResources(0, 1, &Input1);
	context->CSSetShaderResources(1, 1, &Input2);
	context->CSSetUnorderedAccessViews(0, 1, &Result, nullptr);
	context->CSSetShader(ComputeShader, 0, 0);
	context->Dispatch(X, Y, Z);
	context->CSSetShaderResources(0, 1, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	context->CSSetShader(0, 0, 0);
	if (OutputBuffer /*!= nullptr*/ && DebugBuffer /*!= nullptr*/)
	{
		context->CopyResource(DebugBuffer, OutputBuffer);
		static D3D11_MAPPED_SUBRESOURCE MappedResource;
		static HRESULT hr;
		hr = context->Map(DebugBuffer, 0, D3D11_MAP::D3D11_MAP_READ, 0, &MappedResource);
		if (FAILED(hr))
		{
			OutputDebugString(L"Couln't map the debug buffer");
			return;
		}
		for (size_t i = 0; i < DebugSize; ++i)
		{
			auto Pos = ((Data*)MappedResource.pData)[i].Position;
			auto Tex = ((Data*)MappedResource.pData)[i].Texture;
		}
		context->Unmap(DebugBuffer, 0);
	}
}

bool CVertexAdditionShader::CreateBuffers(ID3D11Device * device, Data * data1, Data * data2, size_t size,
	ID3D11Buffer ** InputBuffers, ID3D11Buffer *& OutputBuffer, ID3D11Buffer *& DebugBuffer,
	ID3D11ShaderResourceView ** InputSRVs, ID3D11UnorderedAccessView *& UAV)
{
	HRESULT hr;
	ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, buffDesc);
	ZeroMemoryAndDeclare(D3D11_SUBRESOURCE_DATA, buffData);
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffDesc.ByteWidth = sizeof(Data) * size;
	buffDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc.StructureByteStride = sizeof(Data);
	buffData.pSysMem = data1;
	hr = device->CreateBuffer(&buffDesc, &buffData, &InputBuffers[0]);
	IFFAILED(hr, L"Couldn't create a buffer");
	buffData.pSysMem = data2;
	hr = device->CreateBuffer(&buffDesc, &buffData, &InputBuffers[1]);
	IFFAILED(hr, L"Couldn't create a buffer");
	ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, outputbuffDesc);
	outputbuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	outputbuffDesc.ByteWidth = sizeof(Data) * size;
	outputbuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	outputbuffDesc.StructureByteStride = sizeof(Data);
	outputbuffDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&outputbuffDesc, nullptr, &OutputBuffer);
	IFFAILED(hr, L"Couldn't create a buffer");
	ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, debugbuffDesc);
	debugbuffDesc.BindFlags = 0;
	debugbuffDesc.ByteWidth = sizeof(Data) * size;
	debugbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	debugbuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
	hr = device->CreateBuffer(&debugbuffDesc, nullptr, &DebugBuffer);
	IFFAILED(hr, L"Couldn't create a buffer");
	ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
	srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = size;
	hr = device->CreateShaderResourceView(InputBuffers[0], &srvDesc, &InputSRVs[0]);
	IFFAILED(hr, L"Couldn't create a shader resource view");
	hr = device->CreateShaderResourceView(InputBuffers[1], &srvDesc, &InputSRVs[1]);
	IFFAILED(hr, L"Couldn't create a shader resource view");
	ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc);
	uavDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = size;
	hr = device->CreateUnorderedAccessView(OutputBuffer, &uavDesc, &UAV);
	IFFAILED(hr, L"Couldn't create a unordered access view");
	return true;
}

void CVertexAdditionShader::Shutdown()
{
	SafeRelease(ComputeShader);
}

CVertexAdditionShader::~CVertexAdditionShader()
{
	Shutdown();
}

void CVertexAdditionShader::OutputShaderError(ID3D10Blob * Error)
{
	std::ofstream ofs("ShaderError.txt");
	size_t Size = Error->GetBufferSize();
	char *error = (char*)Error->GetBufferPointer();
	for (size_t i = 0; i < Size; ++i)
		ofs << error[i];
	ofs.close();
}
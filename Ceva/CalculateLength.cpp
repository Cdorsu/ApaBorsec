#include "CalculateLength.h"



CalculateLength::CalculateLength(ID3D11Device * device, ID3D11DeviceContext * immediateContext) :
	m_device(device),
	m_immediateContext(immediateContext)
{
	HRESULT hr;
	ID3DBlob * Shader;
	hr = D3DX11CompileFromFile(L"ComputeLengthComputeShader.hlsl", nullptr, nullptr,
		"main", "cs_4_0", 0, 0, nullptr, &Shader, nullptr, nullptr);
	if (FAILED(hr))
		throw "Fraier";
	hr = m_device->CreateComputeShader(Shader->GetBufferPointer(),
		Shader->GetBufferSize(), nullptr, &m_CalculateLengthShader);
	if (FAILED(hr))
		throw "Fraier";
	SafeRelease(Shader);
}

void CalculateLength::SetData(SData* Data, size_t size)
{
	SafeRelease(m_inputBuffer);
	SafeRelease(m_inputSRV);
	SafeRelease(m_outputBuffer);
	SafeRelease(m_outputUAV);
	SafeRelease(m_debugBuffer);

	HRESULT hr;
	ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, buffDesc);
	ZeroMemoryAndDeclare(D3D11_SUBRESOURCE_DATA, buffData);

	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc.StructureByteStride = sizeof(SData);
	buffDesc.ByteWidth = sizeof(SData) * size;
	buffData.pSysMem = Data;
	hr = m_device->CreateBuffer(&buffDesc, &buffData, &m_inputBuffer);
	if (FAILED(hr))
		throw "fraier";
	ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, inputSRVDesc);
	inputSRVDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	inputSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
	inputSRVDesc.Buffer.ElementOffset = 0;
	inputSRVDesc.Buffer.FirstElement = 0;
	inputSRVDesc.Buffer.NumElements = size;
	inputSRVDesc.Buffer.ElementWidth = sizeof(SData);
	hr = m_device->CreateShaderResourceView(m_inputBuffer, &inputSRVDesc, &m_inputSRV);
	if (FAILED(hr))
		throw "fraier";

	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&buffData, sizeof(D3D11_SUBRESOURCE_DATA));
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc.StructureByteStride = sizeof(float);
	buffDesc.ByteWidth = sizeof(float) * size;
	hr = m_device->CreateBuffer(&buffDesc, nullptr, &m_outputBuffer);
	if (FAILED(hr))
		throw "fraier";
	
	ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, outputUAVDesc);
	outputUAVDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	outputUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_BUFFER;
	outputUAVDesc.Buffer.FirstElement = 0;
	outputUAVDesc.Buffer.Flags = 0;
	outputUAVDesc.Buffer.NumElements = size;
	hr = m_device->CreateUnorderedAccessView(m_outputBuffer, &outputUAVDesc, &m_outputUAV);
	if (FAILED(hr))
		throw "fraier";

	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&buffData, sizeof(D3D11_SUBRESOURCE_DATA));
	buffDesc.BindFlags = 0;
	buffDesc.ByteWidth = sizeof(float) * size;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
	hr = m_device->CreateBuffer(&buffDesc, nullptr, &m_debugBuffer);
	if (FAILED(hr))
		throw "fraier";

	m_size = size;
}

void CalculateLength::Calculate()
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	m_immediateContext->PSSetShaderResources(0, 1, nullSRV);
	m_immediateContext->PSSetShader(0, 0, 0);
	m_immediateContext->CSSetShaderResources(0, 1, &m_inputSRV);
	m_immediateContext->CSSetUnorderedAccessViews(0, 1, &m_outputUAV, nullptr);
	m_immediateContext->CSSetShader(m_CalculateLengthShader, 0, 0);
	m_immediateContext->Dispatch(1, 1, 1);
	m_immediateContext->CSSetShaderResources(0, 1, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	m_immediateContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	m_immediateContext->CSSetShader(0, 0, 0);
}

void CalculateLength::GetResults()
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_immediateContext->CopyResource(m_debugBuffer, m_outputBuffer);
	m_immediateContext->Map(m_debugBuffer, 0, D3D11_MAP::D3D11_MAP_READ, 0, &MappedResource);
	for (size_t i = 0; i < m_size; ++i)
	{
		wchar_t buffer[500] = { 0 };
		swprintf_s(buffer, L"%.2f\n", ((float*)(MappedResource.pData))[i]);
		OutputDebugString(buffer);
	}
	m_immediateContext->Unmap(m_debugBuffer, 0);
}

CalculateLength::~CalculateLength()
{
	SafeRelease(m_CalculateLengthShader);
	SafeRelease(m_inputBuffer);
	SafeRelease(m_inputSRV);
	SafeRelease(m_outputBuffer);
	SafeRelease(m_outputUAV);
	SafeRelease(m_debugBuffer);
}

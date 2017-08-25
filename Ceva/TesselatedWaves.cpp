#include "TesselatedWaves.h"



TesselatedWaves::TesselatedWaves(ID3D11Device * device, ID3D11DeviceContext * immediateContext) :
	m_device(device),
	m_immediateContext(immediateContext)
{
	CreateShaders();
	CreateBuffers();
	mWorld = DirectX::XMMatrixIdentity();
}

void TesselatedWaves::CreateShaders()
{
	ID3D10Blob *ShaderBlob, *ErrorBlob;
	HRESULT hr;
	hr = D3DX11CompileFromFile(L"WavesVertexShader.hlsl", NULL, NULL, "main", "vs_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (ErrorBlob)
		{
			char* Start = (char*)ErrorBlob->GetBufferPointer();
			size_t size = ErrorBlob->GetBufferSize();
			for (size_t i = 0; i < size; ++i)
				OutputDebugStringA(Start);
		}
		throw "Fraier";
	}
	hr = m_device->CreateVertexShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_vertexShader);
	if (FAILED(hr))
		throw "Fraier";

	D3D11_INPUT_ELEMENT_DESC layout[1];
	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].InstanceDataStepRate = 0;
	layout[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InputSlot = 0;
	layout[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[0].AlignedByteOffset = 0;
	hr = m_device->CreateInputLayout(layout, ARRAYSIZE(layout),
		ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), &m_layout);
	if (FAILED(hr))
		throw "Fraier";

	SafeRelease(ShaderBlob);
	SafeRelease(ErrorBlob);
	hr = D3DX11CompileFromFile(L"WavesHullShader.hlsl", NULL, NULL, "main", "hs_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
		throw "Fraier";
	hr = m_device->CreateHullShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_hullShader);
	if (FAILED(hr))
		throw "Fraier";

	SafeRelease(ShaderBlob);
	SafeRelease(ErrorBlob);
	hr = D3DX11CompileFromFile(L"WavesDomainShader.hlsl", NULL, NULL, "main", "ds_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
		throw "Fraier";
	hr = m_device->CreateDomainShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_domainShader);
	if (FAILED(hr))
		throw "Fraier";

	SafeRelease(ShaderBlob);
	SafeRelease(ErrorBlob);
	hr = D3DX11CompileFromFile(L"WavesPixelShader.hlsl", NULL, NULL, "main", "ps_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
		throw "Fraier";
	hr = m_device->CreatePixelShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_pixelShader);
	if (FAILED(hr))
		throw "Fraier";

	SafeRelease(ShaderBlob);
	SafeRelease(ErrorBlob);
}

void TesselatedWaves::CreateBuffers()
{
	HRESULT hr;
	DirectX::XMFLOAT4 Vertices[6] = 
	{
		DirectX::XMFLOAT4(-10.0f, 0.0f, 10.f,1.0f),
		DirectX::XMFLOAT4( 10.0f, 0.0f, 10.f,1.0f),
		DirectX::XMFLOAT4(-10.0f, 0.0f,-10.f,1.0f),
		DirectX::XMFLOAT4( 10.0f, 0.0f,-10.f,1.0f),
	};
	ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, buffDesc);
	ZeroMemoryAndDeclare(D3D11_SUBRESOURCE_DATA, buffData);
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof(Vertices);
	buffData.pSysMem = Vertices;
	hr = m_device->CreateBuffer(&buffDesc, &buffData, &m_vertexBuffer);
	if (FAILED(hr))
		throw "failed";

	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&buffData, sizeof(D3D11_SUBRESOURCE_DATA));
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof(SPerObjectBuffer);
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = m_device->CreateBuffer(&buffDesc, nullptr, &m_PerObject);
	if (FAILED(hr))
		throw "failed";

	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&buffData, sizeof(D3D11_SUBRESOURCE_DATA));
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof(SPerFrame);
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = m_device->CreateBuffer(&buffDesc, nullptr, &m_PerFrame);
	if (FAILED(hr))
		throw "failed";

	ZeroMemory(&buffDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&buffData, sizeof(D3D11_SUBRESOURCE_DATA));
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof(SPerObjectWorld);
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = m_device->CreateBuffer(&buffDesc, nullptr, &m_PerObjectWorld);
	if (FAILED(hr))
		throw "failed";
}

void TesselatedWaves::Render(DirectX::XMFLOAT3 CamPos, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection)
{
	static D3D11_MAPPED_SUBRESOURCE mapped;
	static DirectX::XMMATRIX WVP;
	static UINT Strides = sizeof(DirectX::XMFLOAT4), Offsets = 0;
	m_immediateContext->Map(m_PerObjectWorld, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	((SPerObjectWorld*)mapped.pData)->World = DirectX::XMMatrixTranspose(mWorld);
	m_immediateContext->Unmap(m_PerObjectWorld, 0);
	m_immediateContext->HSSetConstantBuffers(0, 1, &m_PerObjectWorld);

	m_immediateContext->Map(m_PerFrame, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	((SPerFrame*)mapped.pData)->CamPos = CamPos;
	m_immediateContext->Unmap(m_PerFrame, 0);
	m_immediateContext->HSSetConstantBuffers(1, 1, &m_PerFrame);

	WVP = mWorld * View * Projection;
	m_immediateContext->Map(m_PerObject, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	((SPerObjectBuffer*)mapped.pData)->WVP = DirectX::XMMatrixTranspose(WVP);
	m_immediateContext->Unmap(m_PerObject, 0);
	m_immediateContext->DSSetConstantBuffers(0, 1, &m_PerObject);

	m_immediateContext->VSSetShader(m_vertexShader, 0, 0);
	m_immediateContext->HSSetShader(m_hullShader, 0, 0);
	m_immediateContext->DSSetShader(m_domainShader, 0, 0);
	m_immediateContext->PSSetShader(m_pixelShader, 0, 0);

	m_immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &Strides, &Offsets);
	m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	m_immediateContext->IASetInputLayout(m_layout);

	m_immediateContext->Draw(4, 0);

	m_immediateContext->HSSetShader(0, 0, 0);
	m_immediateContext->DSSetShader(0, 0, 0);
}

TesselatedWaves::~TesselatedWaves()
{
	SafeRelease(m_vertexShader);
	SafeRelease(m_hullShader);
	SafeRelease(m_domainShader);
	SafeRelease(m_pixelShader);

	SafeRelease(m_layout);

	SafeRelease(m_vertexBuffer);
	SafeRelease(m_PerFrame);
	SafeRelease(m_PerObject);
	SafeRelease(m_PerObjectWorld);
}

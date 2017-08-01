#include "BillboardShader.h"



CBillboardShader::CBillboardShader()
{
	ZeroMemory(this, sizeof(CBillboardShader));
}

bool CBillboardShader::Initialize(ID3D11Device * device)
{
	ID3D10Blob *ShaderBlob = nullptr, *ErrorBlob = nullptr;
	HRESULT hr = S_OK;
	hr = D3DX11CompileFromFile(L"BillboardVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (ShaderBlob)
			OutputShaderError(ShaderBlob);
		return false;
	}
	//hr = D3DReadFileToBlob( L"VertexShader.cso", &ShaderBlob );
	//if (FAILED( hr ))
	//return false;
	hr = device->CreateVertexShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), NULL, &VertexShader);
	if (FAILED(hr))
		return false;
	D3D11_INPUT_ELEMENT_DESC layout[1];
	layout[0].AlignedByteOffset = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InputSlot = 0;
	layout[0].InstanceDataStepRate = 0;
	layout[0].SemanticIndex = 0;
	layout[0].SemanticName = "POSITION";
	hr = device->CreateInputLayout(layout, sizeof(layout) / sizeof(layout[0]),
		ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), &InputLayout);
	if (FAILED(hr))
		return false;
	SafeRelease(ShaderBlob);
	hr = D3DX11CompileFromFile(L"BillboardPixelShader.hlsl", NULL, NULL, "main", "ps_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (ShaderBlob)
			OutputShaderError(ErrorBlob);
		return false;
	}
	hr = device->CreatePixelShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), NULL, &PixelShader);
	if (FAILED(hr))
		return false;
	hr = D3DX11CompileFromFile(L"BillboardGeometryShader.hlsl", NULL, NULL, "main", "gs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (ShaderBlob)
			OutputShaderError(ErrorBlob);
		return false;
	}
	hr = device->CreateGeometryShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), NULL, &GeometryShader);
	if (FAILED(hr))
		return false;
	D3D11_BUFFER_DESC buffDesc = { 0 };
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof(SConstantBuffer);
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer(&buffDesc, NULL, &ConstantBuffer);
	if (FAILED(hr))
		return false;
	buffDesc.ByteWidth = sizeof(SCamPos);
	hr = device->CreateBuffer(&buffDesc, NULL, &CamBuffer);
	if (FAILED(hr))
		return false;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MipLODBias = 3;
	hr = device->CreateSamplerState(&sampDesc, &Sampler);
	if (FAILED(hr))
		return false;


	return true;
}

void CBillboardShader::Render(ID3D11DeviceContext * context, UINT IndexDrawAmount,
	ID3D11ShaderResourceView * Texture,
	DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
	DirectX::XMFLOAT3 camPos)
{
	using namespace DirectX;
	static HRESULT hr;
	static XMMATRIX WVP;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	context->IASetInputLayout(InputLayout);
	context->VSSetShader(VertexShader, nullptr, 0);
	context->GSSetShader(GeometryShader, nullptr, 0);
	context->PSSetShader(PixelShader, nullptr, 0);
	WVP = World * View * Projection;
	hr = context->Map(ConstantBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(hr))
		return;
	((SConstantBuffer*)MappedResource.pData)->WVP = XMMatrixTranspose(WVP);
	context->Unmap(ConstantBuffer, 0);
	context->GSSetConstantBuffers(0, 1, &ConstantBuffer);
	context->Map(CamBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	((SCamPos*)MappedResource.pData)->camPos = camPos;
	context->Unmap(CamBuffer, 0);
	context->GSSetConstantBuffers(1, 1, &CamBuffer);
	context->PSSetSamplers(0, 1, &Sampler);
	context->PSSetShaderResources(0, 1, &Texture);
	context->DrawIndexed(IndexDrawAmount, 0, 0);
	context->GSSetShader(NULL, 0, 0);
}

void CBillboardShader::OutputShaderError(ID3D10Blob * Error)
{
	std::ofstream ofs("ShaderError.txt");
	UINT Size = (UINT)Error->GetBufferSize();
	char *error = (char*)Error->GetBufferPointer();
	for (UINT i = 0; i < Size; ++i)
		ofs << error[i];
	ofs.close();
}

void CBillboardShader::Shutdown()
{
	SafeRelease(VertexShader);
	SafeRelease(PixelShader);
	SafeRelease(GeometryShader);
	SafeRelease(InputLayout);
	SafeRelease(ConstantBuffer);
	SafeRelease(Sampler);
}

CBillboardShader::~CBillboardShader()
{
	Shutdown();
}

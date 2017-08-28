#include "BlurImage.h"



BlurImage::BlurImage()
{
}

BlurImage::BlurImage(ID3D11Device * device, ID3D11DeviceContext * context, ID3D11ShaderResourceView * SRV, ID3D11UnorderedAccessView *& UAV) :
	m_immediateContext(context),
	m_device(device)
{
	if (!SetResource(SRV, UAV))
		throw "Fraier";
	ID3D10Blob *ShaderBlob, *ErrorBlob;
	HRESULT hr;
	hr = D3DX11CompileFromFile(L"HorizontalBlurComputeShader.hlsl", NULL, NULL, "main", "cs_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
		throw "Fraier";
	hr = device->CreateComputeShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_horizontalBlurShader);
	if (FAILED(hr))
		throw "Fraier";
	SafeRelease(ShaderBlob);
	hr = D3DX11CompileFromFile(L"VerticalBlurComputeShader.hlsl", NULL, NULL, "main", "cs_5_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL);
	if (FAILED(hr))
		throw "Fraier";
	hr = device->CreateComputeShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_verticalBlurShader);
	if (FAILED(hr))
		throw "Fraier";
	SafeRelease(ShaderBlob);
	
	ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, buffDesc);
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof(BlurInfo);
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	hr = device->CreateBuffer(&buffDesc, nullptr, &m_blurInfo);
	if (FAILED(hr))
		throw "Fraier";
	CalculateWeights(MAXBLURRADIUS, DEFAULTSIGMA);
}


BlurImage::~BlurImage()
{
	ClearResources();
	SafeRelease(m_horizontalBlurShader);
	SafeRelease(m_verticalBlurShader);
	SafeRelease(m_blurInfo);
}

bool BlurImage::SetResource(ID3D11ShaderResourceView * newResource, ID3D11UnorderedAccessView *& newUAV)
{
	HRESULT hr;
	
	ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, inputDesc);
	ID3D11Texture2D * inputCopyTexture = nullptr;
	ID3D11Texture2D * inputTexture = nullptr;
	newResource->GetResource(reinterpret_cast<ID3D11Resource**>(&inputTexture));
	inputTexture->GetDesc(&inputDesc);
	hr = m_device->CreateTexture2D(&inputDesc, nullptr, &inputCopyTexture);
	m_immediateContext->CopyResource(reinterpret_cast<ID3D11Resource*>(inputCopyTexture),
		reinterpret_cast<ID3D11Resource*>(inputTexture));
	ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, inputSRVDesc);
	inputSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	inputSRVDesc.Format = inputDesc.Format;
	inputSRVDesc.Texture2D.MipLevels = inputDesc.MipLevels;
	inputSRVDesc.Texture2D.MostDetailedMip = 0;
	hr = m_device->CreateShaderResourceView(inputCopyTexture, &inputSRVDesc, &m_inputTexture);
	ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, inputUAVDesc);
	inputUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	inputUAVDesc.Format = inputDesc.Format;
	inputUAVDesc.Texture2D.MipSlice = 1;
	hr = m_device->CreateUnorderedAccessView(inputCopyTexture, &inputUAVDesc, &newUAV);
	IFFAILED(hr, L"Fraier");
	m_inputAccess = newUAV;

	ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, outputDesc);
	outputDesc.ArraySize = inputDesc.ArraySize;
	outputDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | 
		D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.Format = inputDesc.Format;
	outputDesc.Width = inputDesc.Width;
	outputDesc.Height = inputDesc.Height;
	outputDesc.MipLevels = inputDesc.MipLevels;
	outputDesc.SampleDesc = inputDesc.SampleDesc;
	outputDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	
	ID3D11Texture2D * outputTexture;
	hr = m_device->CreateTexture2D(&outputDesc, nullptr, &outputTexture);
	IFFAILED(hr, L"Fraier");

	ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, outputSRVDesc);
	outputSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2D;
	outputSRVDesc.Format = inputDesc.Format;
	outputSRVDesc.Texture2D.MipLevels = inputDesc.MipLevels;
	outputSRVDesc.Texture2D.MostDetailedMip = 0;
	hr = m_device->CreateShaderResourceView(outputTexture, &outputSRVDesc, &m_outputTexture);
	IFFAILED(hr, L"Fraier");
	ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, outputUAVDesc);
	outputUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	outputUAVDesc.Format = inputDesc.Format;
	outputUAVDesc.Texture2D.MipSlice = 0;
	hr = m_device->CreateUnorderedAccessView(outputTexture, &outputUAVDesc, &m_outputAccess);
	IFFAILED(hr, L"Fraier");

	float hGroups = float(inputDesc.Width) / float(GROUPWIDTH);
	float vGroups = float(inputDesc.Height) / float(GROUPHEIGHT);
	m_horizontalGroups = (int)ceilf(hGroups);
	m_verticalGroups = (int)ceilf(vGroups);

	mWidth = inputDesc.Width;
	mHeight = inputDesc.Height;

	SafeRelease(outputTexture);
	SafeRelease(inputCopyTexture);

	return true;
}

void BlurImage::PerformBlur()
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	m_immediateContext->PSSetShaderResources(0, 1, nullSRV);
	m_immediateContext->PSSetShader(0, 0, 0);
	m_immediateContext->CSSetShader(m_horizontalBlurShader, 0, 0);
	//m_immediateContext->CSSetShaderResources(0, 1, &m_inputTexture);
	//m_immediateContext->CSSetUnorderedAccessViews(0, 1, &m_outputAccess, nullptr);
	//m_immediateContext->Dispatch(m_horizontalGroups, mHeight, 1);

	for (int i = 0; i < 3; ++i)
	{
		m_immediateContext->CSSetShader(m_horizontalBlurShader, 0, 0);
		m_immediateContext->CSSetShaderResources(0, 1, &m_inputTexture);
		m_immediateContext->CSSetUnorderedAccessViews(0, 1, &m_outputAccess, nullptr);
		m_immediateContext->Dispatch(m_horizontalGroups, mHeight, 1);

		m_immediateContext->CSSetShaderResources(0, 1, nullSRV);
		m_immediateContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

		ID3D11Resource *output, *input;
		m_inputTexture->GetResource(&input);
		m_outputTexture->GetResource(&output);
		m_immediateContext->CopyResource(input, output);

		m_immediateContext->CSSetShader(m_verticalBlurShader, 0, 0);
		m_immediateContext->CSSetShaderResources(0, 1, &m_inputTexture);
		m_immediateContext->CSSetUnorderedAccessViews(0, 1, &m_outputAccess, nullptr);
		m_immediateContext->Dispatch(mWidth, m_verticalGroups, 1);

		m_immediateContext->CSSetShaderResources(0, 1, nullSRV);
		m_immediateContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	m_immediateContext->CSSetShaderResources(0, 1, nullSRV);
	m_immediateContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	m_immediateContext->CSSetShader(0, 0, 0);

}

void BlurImage::CalculateWeights(int radius, float sigma)
{
	m_blur.radius = radius;
	float d = 2 * sigma * sigma;
	
	float sum = 0;
	for (int i = 0; i < radius; ++i)
	{
		float x = (float)i;
		m_blur.weights[i] = expf((-x*x) / d);
		sum += m_blur.weights[i];
	}
	for (int i = 0; i < radius; ++i)
		m_blur.weights[i] /= sum;
	
	m_immediateContext->UpdateSubresource(m_blurInfo, 0, nullptr, &m_blur, 0, 0);
	m_immediateContext->CSSetConstantBuffers(0, 1, &m_blurInfo);
}

void BlurImage::ScreenShot()
{
	static int count = 0;
	count++;
	wchar_t buffer[500];
	wsprintf(buffer, L"SuntUnAdevaratGeniu%02d.png", count);
	ID3D11Resource * RS;
	m_outputTexture->GetResource(&RS);
	D3DX11SaveTextureToFile(m_immediateContext, RS, D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_PNG, buffer);
}

void BlurImage::ClearResources()
{
	SafeRelease(m_outputTexture);
	SafeRelease(m_outputAccess);
}

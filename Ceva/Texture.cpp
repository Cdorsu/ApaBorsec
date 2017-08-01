#include "Texture.h"



CTexture::CTexture()
{
	ZeroMemory( this, sizeof( CTexture ) );
}

bool CTexture::Initialize( ID3D11Device * device, LPWSTR filepath )
{
	HRESULT hr;

	hr = D3DX11CreateShaderResourceViewFromFile( device, filepath, NULL, NULL, &Texture, NULL );
	if (FAILED( hr ))
	{
		MessageBox( NULL, L"Couldn't get texture from file",
			L"Error", MB_ICONERROR | MB_OK );
		return false;
	}

	return true;
}

bool CTexture::Initialize(ID3D11Device * device, ID3D11DeviceContext * context,std::vector<LPWSTR> Paths)
{
	HRESULT hr;
	size_t size = Paths.size();
	std::vector<ID3D11Texture2D*> Textures(size);
	for (size_t i = 0; i < size; ++i)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.Filter = D3DX11_FILTER_FLAG::D3DX11_FILTER_POINT;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipFilter = D3DX11_FILTER_FLAG::D3DX11_FILTER_POINT;
		loadInfo.MipLevels = 0;
		loadInfo.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
		loadInfo.pSrcInfo = 0;
		loadInfo.MiscFlags = 0;
		hr = D3DX11CreateTextureFromFile(device, Paths[i], &loadInfo, nullptr, (ID3D11Resource**)&Textures[i], nullptr);
		IFFAILED(hr, L"");
	}

	D3D11_TEXTURE2D_DESC texDesc;
	Textures[0]->GetDesc(&texDesc);

	ID3D11Texture2D* texArray;
	texDesc.ArraySize = size;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.CPUAccessFlags = 0;

	hr = device->CreateTexture2D(&texDesc, nullptr, &texArray);
	IFFAILED(hr, L"");
	for (size_t texture = 0; texture < size; ++texture)
	{
		for (size_t mip = 0; mip < texDesc.MipLevels; ++mip)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTexture;
			context->Map(Textures[texture], 0, D3D11_MAP::D3D11_MAP_READ, 0, &mappedTexture);

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mip,texture,texDesc.MipLevels),
				nullptr, mappedTexture.pData, mappedTexture.RowPitch, mappedTexture.DepthPitch);

			context->Unmap(Textures[texture], 0);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = size;
	srvDesc.Texture2DArray.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(texArray, &srvDesc, &Texture);
	IFFAILED(hr, L"");

	//SafeRelease(texArray);

	for (size_t i = 0; i < size; ++i)
		SafeRelease(Textures[i]);

	return true;
}

void CTexture::Shutdown()
{
	SafeRelease( Texture );
}

CTexture::~CTexture()
{
	Shutdown();
}

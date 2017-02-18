#include "RenderTexture.h"



CRenderTexture::CRenderTexture()
{
	ZeroMemory( this, sizeof( CRenderTexture ) );
}

bool CRenderTexture::Initialize( ID3D11Device * device, UINT WindowWidth, UINT WindowHeight, float FOV, float ScreenNear, float ScreenDepth )
{
	TextureWidth = ( FLOAT ) WindowWidth;
	TextureHeight = ( FLOAT ) WindowHeight;

	HRESULT hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory( &srvDesc, sizeof( srvDesc ) );
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory( &texDesc, sizeof( texDesc ) );
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory( &texDesc, sizeof( rtvDesc ) );

	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = WindowWidth;
	texDesc.Height = WindowHeight;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	hr = device->CreateTexture2D( &texDesc, NULL, &Texture );
	if (FAILED( hr ))
		return false;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView( Texture, &rtvDesc, &RenderTargetView );
	if (FAILED( hr ))
		return false;

	srvDesc.Format = texDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = device->CreateShaderResourceView( Texture, &srvDesc, &ShaderResourceView );
	if (FAILED( hr ))
		return false;

	ID3D11Texture2D *DSBuffer = nullptr;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.Width = WindowWidth;
	texDesc.Height = WindowHeight;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	hr = device->CreateTexture2D( &texDesc, NULL, &DSBuffer );
	if ( FAILED( hr ) )
		return false;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView( DSBuffer, &dsvDesc, &DSView );
	SafeRelease( DSBuffer );

	Projection = DirectX::XMMatrixPerspectiveFovLH( FOV,
		( FLOAT ) WindowWidth / WindowHeight, ScreenNear, ScreenDepth );
	Ortographic = DirectX::XMMatrixOrthographicLH(
		( FLOAT ) WindowWidth, ( FLOAT ) WindowHeight, ScreenNear, ScreenDepth );

	ViewPort.Width = TextureWidth;
	ViewPort.Height = TextureHeight;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.MaxDepth = 1.0f;
	ViewPort.MinDepth = 0.0f;

	return true;
}

void CRenderTexture::Shutdown()
{
	SafeRelease( Texture );
	SafeRelease( RenderTargetView );
	SafeRelease( ShaderResourceView );
	SafeRelease( DSView );
}

CRenderTexture::~CRenderTexture()
{
	Shutdown();
}

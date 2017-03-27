#include "RenderTexture.h"



CRenderTexture::CRenderTexture( )
{
	ZeroMemory( this, sizeof( CRenderTexture ) );
}

bool CRenderTexture::Initialize( ID3D11Device * device, UINT WindowWidth, UINT WindowHeight, float FOV, float ScreenNear, float ScreenDepth )
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC texDesc, depthviewDesc;
	ZeroMemory( &texDesc, sizeof( texDesc ) );
	ZeroMemory( &depthviewDesc, sizeof( depthviewDesc ) );
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory( &rtvDesc, sizeof( rtvDesc ) );
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory( &srvDesc, sizeof( srvDesc ) );
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory( &dsDesc, sizeof( dsDesc ) );
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory( &dsvDesc, sizeof( dsvDesc ) );

	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = WindowWidth;
	texDesc.Height = WindowHeight;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	hr = device->CreateTexture2D( &texDesc, NULL, &Texture );
	if ( FAILED( hr ) )
		return false;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = device->CreateRenderTargetView( Texture, &rtvDesc, &RenderTargetView );
	if ( FAILED( hr ) )
		return false;

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView( Texture, &srvDesc, &ShaderResourceView );
	if ( FAILED( hr ) )
		return false;

	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;

	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

	hr = device->CreateDepthStencilState( &dsDesc, &DSState );
	if ( FAILED( hr ) )
		return false;

	ID3D11Texture2D *depthviewTexture2d;
	depthviewDesc.ArraySize = 1;
	depthviewDesc.MipLevels = 1;
	depthviewDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	depthviewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthviewDesc.Height = WindowHeight;
	depthviewDesc.Width = WindowWidth;
	depthviewDesc.SampleDesc.Count = 1;
	depthviewDesc.SampleDesc.Quality = 0;
	depthviewDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	hr = device->CreateTexture2D( &depthviewDesc, NULL, &depthviewTexture2d );
	if ( FAILED( hr ) )
		return false;

	dsvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView( depthviewTexture2d, &dsvDesc, &DSView );
	if ( FAILED( hr ) )
		return false;

	depthviewTexture2d->Release( );

	ViewPort.Height = ( FLOAT ) WindowHeight;
	ViewPort.Width = ( FLOAT ) WindowWidth;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.MinDepth = 0.0f;
	ViewPort.MaxDepth = 1.0f;

	Ortographic = DirectX::XMMatrixOrthographicLH( ( FLOAT ) WindowWidth, ( FLOAT ) WindowHeight, ScreenNear, ScreenDepth );
	Projection = DirectX::XMMatrixPerspectiveFovLH( FOV, ( FLOAT ) WindowWidth / WindowHeight, ScreenNear, ScreenDepth );
	
	return true;
}

void CRenderTexture::SetRenderTarget( ID3D11DeviceContext * context )
{
	context->RSSetViewports( 1, &ViewPort );
	context->OMSetRenderTargets( 1, &RenderTargetView, DSView );
}

void CRenderTexture::BeginScene( ID3D11DeviceContext * context, common::Color& back )
{
	context->ClearRenderTargetView( RenderTargetView, back );
	context->ClearDepthStencilView( DSView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH |
		D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

ID3D11ShaderResourceView* CRenderTexture::GetTexture( )
{
	return ShaderResourceView;
}

void CRenderTexture::Shutdown( )
{
	SafeRelease( Texture );
	SafeRelease( RenderTargetView );
	SafeRelease( ShaderResourceView );
	SafeRelease( DSView );
	SafeRelease( DSState );
}

CRenderTexture::~CRenderTexture( )
{
	Shutdown( );
}
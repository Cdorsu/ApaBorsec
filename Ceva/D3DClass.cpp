#include "D3DClass.h"


common::Color D3DClass::BackgroundColor = common::Color( 0.5f, 0.5f, 0.2f, 1.0f );


D3DClass::D3DClass()
{
	ZeroMemory( this, sizeof( D3DClass ) );
}

bool D3DClass::Initialize( HINSTANCE hInstance, HWND hWnd, UINT Width, UINT Height, FLOAT NearZ, FLOAT FarZ )
{
	HRESULT hr;
	std::wofstream ofs( L"Information.txt" );
	IDXGIFactory *Factory;
	IDXGIAdapter *Adapter;
	IDXGIOutput *Output;
	DXGI_MODE_DESC *ModeDesc, buffDesc;
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	DXGI_ADAPTER_DESC AdapterDesc;
	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	UINT totalModes = 0, Numerator,Denominator;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	hr = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)&Factory );
	if (FAILED( hr ))
		return false;
	hr = Factory->EnumAdapters( 0, &Adapter );
	if (FAILED( hr ))
		return false;
	hr = Adapter->EnumOutputs( 0, &Output );
	if (FAILED( hr ))
		return false;
	hr = Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &totalModes, NULL );
	if (FAILED( hr ))
		return false;
	ModeDesc = new DXGI_MODE_DESC[totalModes];
	hr = Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &totalModes, ModeDesc );
	if (FAILED( hr ))
		return false;
	for (UINT i = 0; i < totalModes; ++i)
		if (Width == ModeDesc[i].Width && Height == ModeDesc[i].Height)
		{
			Numerator = ModeDesc[i].RefreshRate.Numerator;
			Denominator = ModeDesc[i].RefreshRate.Denominator;
		}
	delete ModeDesc;
	hr = Adapter->GetDesc( &AdapterDesc );
	if (FAILED( hr ))
		return false;
	m_GPU = AdapterDesc.Description;
	m_VideoMemory = (UINT)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);
	ofs << L"Video card: " << m_GPU << L" with " << m_VideoMemory << L" megabytes of dedicated video memory\n";
	ofs.close();

	buffDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	buffDesc.Width = Width;
	buffDesc.Height = Height;
	buffDesc.RefreshRate.Numerator = Numerator;
	buffDesc.RefreshRate.Denominator = Denominator;
	buffDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
	buffDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc = buffDesc;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Windowed = TRUE;

	UINT flags = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_SINGLETHREADED;

#if DEBUG || _DEBUG
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, NULL, flags,
		NULL, 0, D3D11_SDK_VERSION, &SwapChainDesc, &m_SwapChain, &m_d3d11Device, NULL, &m_d3d11DeviceContext);
	if (FAILED( hr ))
		return false;

	// Render Target View
	ID3D11Texture2D *Backbuffer = nullptr;
	hr = m_SwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&Backbuffer );
	if (FAILED( hr ))
		return false;
	hr = m_d3d11Device->CreateRenderTargetView( Backbuffer, NULL, &m_RenderTargetView );
	if (FAILED( hr ))
		return false;
	Backbuffer->Release();
	
	// Depth Stencil View
	ID3D11Texture2D *DSBuffer = nullptr;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.Width = Width;
	texDesc.Height = Height;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	texDesc.MipLevels = 1;
	hr = m_d3d11Device->CreateTexture2D( &texDesc, NULL, &DSBuffer );
	if (FAILED( hr ))
		return false;
	ZeroMemory( &dsViewDesc, sizeof( dsViewDesc ) );
	dsViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Texture2D.MipSlice = 0;
	hr = m_d3d11Device->CreateDepthStencilView( DSBuffer, &dsViewDesc, &m_DepthStencilView );
	if (FAILED( hr ))
		return false;
	DSBuffer->Release();

	ZeroMemory( &dsDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
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

	hr = m_d3d11Device->CreateDepthStencilState( &dsDesc, &DSDefaultState );
	if ( FAILED( hr ) )
		return false;

	dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	hr = m_d3d11Device->CreateDepthStencilState( &dsDesc, &DSLessEqual );
	if ( FAILED( hr ) )
		return false;

	m_d3d11DeviceContext->OMSetDepthStencilState( DSDefaultState, 0 );

	m_d3d11DeviceContext->OMSetRenderTargets( 1, &m_RenderTargetView, m_DepthStencilView );

	DefaultViewPort.Height = (FLOAT)Height;
	DefaultViewPort.Width = (FLOAT)Width;
	DefaultViewPort.MaxDepth = 1.0f;
	DefaultViewPort.MinDepth = 0.0f;
	DefaultViewPort.TopLeftX = 0;
	DefaultViewPort.TopLeftY = 0;
	m_d3d11DeviceContext->RSSetViewports( 1, &DefaultViewPort );

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory( &rastDesc, sizeof( rastDesc ) );
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	hr = m_d3d11Device->CreateRasterizerState( &rastDesc, &NoCulling );
	if (FAILED( hr ))
		return false;
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	hr = m_d3d11Device->CreateRasterizerState( &rastDesc, &Wireframe );
	if ( FAILED( hr ) )
		return false;

	D3D11_BLEND_DESC blend;
	ZeroMemory( &blend, sizeof( blend ) );
	blend.IndependentBlendEnable = FALSE;
	blend.AlphaToCoverageEnable = FALSE;
	blend.RenderTarget[ 0 ].BlendEnable = TRUE;
	blend.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	blend.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	blend.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	blend.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	blend.RenderTarget[ 0 ].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	blend.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	blend.RenderTarget[ 0 ].RenderTargetWriteMask = 0;
	hr = m_d3d11Device->CreateBlendState( &blend, &AlphaBlendingEnabled );
	if (FAILED( hr ))
		return false;
	ZeroMemory( &blend, sizeof( blend ) );
	blend.RenderTarget[ 0 ].BlendEnable = FALSE;
	blend.RenderTarget[ 0 ].RenderTargetWriteMask = 0;
	hr = m_d3d11Device->CreateBlendState( &blend, &NoRenderTargetWrite );
	if ( FAILED( hr ) )
		return false;

	// D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory( &dsDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	// Not used
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	hr = m_d3d11Device->CreateDepthStencilState( &dsDesc, &DSWriteStencil );
	if ( FAILED( hr ) )
		return false;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
	// Not used
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
	hr = m_d3d11Device->CreateDepthStencilState( &dsDesc, &DSDrawReflection );
	if ( FAILED( hr ) )
		return false;

	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[ 0 ].BlendEnable = true;
	transparentDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_d3d11Device->CreateBlendState( &transparentDesc, &Transparency );
	if ( FAILED( hr ) )
		return false;
	
	D3D11_BLEND_DESC shadowblendDesc = { 0 };
	shadowblendDesc.AlphaToCoverageEnable = false;
	shadowblendDesc.IndependentBlendEnable = false;

	shadowblendDesc.RenderTarget[ 0 ].BlendEnable = true;
	shadowblendDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	shadowblendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_SRC_COLOR;
	shadowblendDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	shadowblendDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ZERO;
	shadowblendDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
	shadowblendDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	shadowblendDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_d3d11Device->CreateBlendState( &shadowblendDesc, &ShadowRendering );
	if ( FAILED( hr ) )
		return false;

	D3D11_BLEND_DESC alphaDesc = { 0 };
	alphaDesc.AlphaToCoverageEnable = true;
	alphaDesc.IndependentBlendEnable = false;
	alphaDesc.RenderTarget[0].BlendEnable = false;
	alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_d3d11Device->CreateBlendState(&alphaDesc, &AlphaToCoverage);
	if (FAILED(hr))
		return false;

	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
	// Not used
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
	hr = m_d3d11Device->CreateDepthStencilState( &dsDesc, &DSDrawShadow );
	if ( FAILED( hr ) )
		return false;

	Output->Release();
	Factory->Release();
	Adapter->Release();

	m_OrthoMatrix = DirectX::XMMatrixOrthographicLH( (FLOAT)Width, (FLOAT)Height, NearZ, FarZ );

	return true;
}

void D3DClass::Shutdown()
{
	SafeRelease( m_SwapChain );
	SafeRelease( m_d3d11Device );
	SafeRelease( m_d3d11DeviceContext );
	SafeRelease( m_DepthStencilView );
	SafeRelease( m_RenderTargetView );
	SafeRelease( NoCulling );
	SafeRelease( Wireframe );
	SafeRelease( DSDefaultState );
	SafeRelease( DSLessEqual );
	SafeRelease( NoRenderTargetWrite );
	SafeRelease( DSWriteStencil );
	SafeRelease( DSDrawReflection );
	SafeRelease( DSDrawShadow );
	SafeRelease( Transparency );
	SafeRelease( ShadowRendering );
	SafeRelease(AlphaToCoverage);
}

void D3DClass::BeginScene()
{
	m_d3d11DeviceContext->ClearRenderTargetView( m_RenderTargetView, BackgroundColor );
	m_d3d11DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

void D3DClass::EndScene()
{
#ifdef VSYNC
	m_SwapChain->Present( 1, 0 );
#else
	m_SwapChain->Present( 0, 0 );
#endif // VSYNC

}

D3DClass::~D3DClass()
{
	Shutdown();
}

#include "CubeMapping.h"



CubeMapping::CubeMapping( CTextureShader * shader,
	ID3D11Device * device, ID3D11DeviceContext * immediateContext ) :
	m_device( device ),
	m_immediateContext( immediateContext ),
	m_textureShader( shader )
{
	CreateModels( );
	CreateShaders( );
	CreateBuffers( );
	CreateRTVS( );
	CreateMatrices( );
}

void CubeMapping::CreateModels( )
{
	m_model = new CModel( );
	if ( !m_model->Initialize( m_device, L"data\\sphere.txt", L"data\\marble01.dds" ) )
		throw "Fraier";
	m_sky = new CModel( );
	if ( !m_sky->Initialize( m_device ) )
		throw "Fraier";
	m_floor = new CModel( );
	if ( !m_floor->Initialize( m_device, L"data\\floor.txt", L"data\\ground01.dds" ) )
		throw "Fraier";
	m_floor->Translate( 0.0f, -1.0f, 0.0f );
	m_floor->Scale( 10, 10, 10 );

	HRESULT hr;
	ID3D11Texture2D * Texture = nullptr;
	D3DX11_IMAGE_LOAD_INFO imgInfo;
	imgInfo.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = D3DX11CreateTextureFromFile( m_device, L"data\\city.dds", &imgInfo, nullptr,
		reinterpret_cast< ID3D11Resource** >( &Texture ), nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	D3D11_TEXTURE2D_DESC texDesc;
	Texture->GetDesc( &texDesc );
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory( &srvDesc, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
	srvDesc.Format = texDesc.Format;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBE;
	hr = m_device->CreateShaderResourceView( Texture, &srvDesc, &m_textureSRV );
	if ( FAILED( hr ) )
		throw "Fraier";
	SafeRelease( Texture );
}

void CubeMapping::CreateShaders( )
{
	HRESULT hr;
	ID3DBlob *Shader, *Errors;
	hr = D3DX11CompileFromFile( L"CubeMappingVertexShader.hlsl", nullptr, nullptr,
		"main", "vs_4_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = m_device->CreateVertexShader( Shader->GetBufferPointer( ),
		Shader->GetBufferSize( ),
		nullptr, &m_vertexShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	D3D11_INPUT_ELEMENT_DESC layout[ 3 ];
	layout[ 0 ].AlignedByteOffset = 0;
	layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 0 ].InputSlot = 0;
	layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 0 ].InstanceDataStepRate = 0;
	layout[ 0 ].SemanticIndex = 0;
	layout[ 0 ].SemanticName = "POSITION";
	layout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 1 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	layout[ 1 ].InputSlot = 0;
	layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 1 ].InstanceDataStepRate = 0;
	layout[ 1 ].SemanticIndex = 0;
	layout[ 1 ].SemanticName = "TEXCOORD";
	layout[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 2 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 2 ].InputSlot = 0;
	layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 2 ].InstanceDataStepRate = 0;
	layout[ 2 ].SemanticIndex = 0;
	layout[ 2 ].SemanticName = "NORMAL";

	int numElements = ARRAYSIZE( layout );

	hr = m_device->CreateInputLayout( layout, numElements, Shader->GetBufferPointer( ),
		Shader->GetBufferSize( ), &m_inputLayout );
	if ( FAILED( hr ) )
		throw "Failed";

	SafeRelease( Shader );
	SafeRelease( Errors );
	hr = D3DX11CompileFromFile( L"CubeMappingPixelShader.hlsl", nullptr, nullptr,
		"main", "ps_4_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = m_device->CreatePixelShader( Shader->GetBufferPointer( ),
		Shader->GetBufferSize( ),
		nullptr, &m_pixelShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	SafeRelease( Shader );
	SafeRelease( Errors );

	hr = D3DX11CompileFromFile( L"SkyVertexShader.hlsl", nullptr, nullptr,
		"main", "vs_4_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = m_device->CreateVertexShader( Shader->GetBufferPointer( ),
		Shader->GetBufferSize( ),
		nullptr, &m_SkyVertexShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	D3D11_INPUT_ELEMENT_DESC slayout[ 1 ];
	slayout[ 0 ].AlignedByteOffset = 0;
	slayout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	slayout[ 0 ].InputSlot = 0;
	slayout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	slayout[ 0 ].InstanceDataStepRate = 0;
	slayout[ 0 ].SemanticIndex = 0;
	slayout[ 0 ].SemanticName = "POSITION";
	UINT numLayout2 = ARRAYSIZE( slayout );
	hr = m_device->CreateInputLayout( slayout, numLayout2,
		Shader->GetBufferPointer( ), Shader->GetBufferSize( ), &m_SkyInputLayout );
	if ( FAILED( hr ) )
		throw "Fraier";

	SafeRelease( Shader );
	SafeRelease( Errors );
	hr = D3DX11CompileFromFile( L"SkyPixelShader.hlsl", nullptr, nullptr,
		"main", "ps_4_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = m_device->CreatePixelShader( Shader->GetBufferPointer( ),
		Shader->GetBufferSize( ),
		nullptr, &m_SkyPixelShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	SafeRelease( Shader );
	SafeRelease( Errors );
}

void CubeMapping::CreateBuffers( )
{
	HRESULT hr;
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, buffDesc );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SCBPerObject );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = m_device->CreateBuffer( &buffDesc, nullptr, &m_perObjectBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";
	buffDesc.ByteWidth = sizeof( SCBPerFrame );
	hr = m_device->CreateBuffer( &buffDesc, nullptr, &m_perFrameBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemoryAndDeclare( D3D11_SAMPLER_DESC, sampDesc );
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MipLODBias = 3;
	hr = m_device->CreateSamplerState( &sampDesc, &m_sampler );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemoryAndDeclare( D3D11_DEPTH_STENCIL_DESC, dsDesc );
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = FALSE;
	hr = m_device->CreateDepthStencilState( &dsDesc, &m_DSState );
	if ( FAILED( hr ) )
		throw "Fraier";
}

void CubeMapping::CreateRTVS( )
{
	HRESULT hr;
	ID3D11Texture2D * CubemapTexture = nullptr;
	ZeroMemoryAndDeclare( D3D11_TEXTURE2D_DESC, texDesc );
	texDesc.ArraySize = 6;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = CubemapTextureSize;
	texDesc.Height = CubemapTextureSize;
	texDesc.MipLevels = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_GENERATE_MIPS |
		D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	hr = m_device->CreateTexture2D( &texDesc, nullptr, &CubemapTexture );
	if ( FAILED( hr ) )
		throw "Fraier";
	ZeroMemoryAndDeclare( D3D11_RENDER_TARGET_VIEW_DESC, rtvDesc );
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;
	for ( int i = 0; i < 6; ++i )
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		hr = m_device->CreateRenderTargetView( CubemapTexture, &rtvDesc, &m_DynamicRenderTargets[ i ] );
		if ( FAILED( hr ) )
			throw "Fraier";
	}
	ZeroMemoryAndDeclare( D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc );
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;
	hr = m_device->CreateShaderResourceView( CubemapTexture, &srvDesc, &m_CubemapTexture );
	if ( FAILED( hr ) )
		throw "Fraier";
	SafeRelease( CubemapTexture );

	ID3D11Texture2D * DepthResource = nullptr;
	ZeroMemory( &texDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
	texDesc.ArraySize = 6;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.Width = CubemapTextureSize;
	texDesc.Height = CubemapTextureSize;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = m_device->CreateTexture2D( &texDesc, nullptr, &DepthResource );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemoryAndDeclare( D3D11_DEPTH_STENCIL_VIEW_DESC, dsvDesc );
	dsvDesc.Format = texDesc.Format;
	dsvDesc.Texture2DArray.ArraySize = 1;
	dsvDesc.Texture2DArray.FirstArraySlice = 0;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	hr = m_device->CreateDepthStencilView( DepthResource, &dsvDesc, &m_CubemapDSView );
	if ( FAILED( hr ) )
		throw "Fraier";

	m_CubemapViewport.TopLeftX = 0;
	m_CubemapViewport.TopLeftY = 0;
	m_CubemapViewport.Width = CubemapTextureSize;
	m_CubemapViewport.Height = CubemapTextureSize;
	m_CubemapViewport.MinDepth = 0.0f;
	m_CubemapViewport.MaxDepth = 1.0f;
}

void CubeMapping::CreateMatrices( )
{
	float x, y, z;
	x = 0.0f, y = 0.0f, z = 0.0f;
	DirectX::XMFLOAT3 LookTos[ 6 ];
	LookTos[ 0 ] = DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f );
	LookTos[ 1 ] = DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f );
	LookTos[ 2 ] = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f );
	LookTos[ 3 ] = DirectX::XMFLOAT3( 0.0f, -1.0f, 0.0f );
	LookTos[ 4 ] = DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f );
	LookTos[ 5 ] = DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f );
	DirectX::XMFLOAT3 Ups[ 6 ];
	Ups[ 0 ] = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f );
	Ups[ 1 ] = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f );
	Ups[ 2 ] = DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f );
	Ups[ 3 ] = DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f );
	Ups[ 4 ] = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f );
	Ups[ 5 ] = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f );

	m_Projection = DirectX::XMMatrixPerspectiveFovLH( ( float ) D3DX_PI / 2.0f, 1, 0.1f, 100.0f );

	for ( int i = 0; i < 6; ++i )
	{
		DirectX::XMVECTOR Position;
		Position = DirectX::XMVectorSet( x, y, z, 1.0f );
		DirectX::XMVECTOR Direction;
		Direction = DirectX::XMLoadFloat3( &LookTos[ i ] );
		DirectX::XMVECTOR Up;
		Up = DirectX::XMLoadFloat3( &Ups[ i ] );
		m_Views[ i ] = DirectX::XMMatrixLookToLH( Position, Direction, Up );
	}
}

void CubeMapping::Render( DirectX::XMFLOAT3 EyePos, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
{
	static D3D11_MAPPED_SUBRESOURCE Mapped;
	m_immediateContext->PSSetSamplers( 0, 1, &m_sampler );

	m_floor->Render( m_immediateContext );

	DirectX::XMMATRIX v = View;
	DirectX::XMMATRIX p = Projection;
	m_textureShader->Render( m_immediateContext, m_floor->GetIndexCount( ),
		m_floor->GetTexture( ), m_floor->GetWorld( ), v, p, nullptr, nullptr );

	D3D11_VIEWPORT oldViewport;
	UINT numViewports = 1;
	m_immediateContext->RSGetViewports( &numViewports, &oldViewport );
	ID3D11RenderTargetView * oldRTV = nullptr;
	ID3D11DepthStencilView * oldDS = nullptr;
	m_immediateContext->OMGetRenderTargets( 1, &oldRTV, &oldDS );

	for ( int i = 0; i < 6; ++i )
	{
		m_immediateContext->RSSetViewports( 1, &m_CubemapViewport );
		m_immediateContext->OMSetRenderTargets( 1, &m_DynamicRenderTargets[ i ], m_CubemapDSView );
		FLOAT BLACK[ 4 ] = { 1.f,1.f,0.f,0.f };
		m_immediateContext->ClearRenderTargetView( m_DynamicRenderTargets[ i ], BLACK );
		m_immediateContext->ClearDepthStencilView( m_CubemapDSView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0 );

		m_floor->Render( m_immediateContext );
		m_textureShader->Render( m_immediateContext, m_floor->GetIndexCount( ),
			m_floor->GetTexture( ), m_floor->GetWorld( ), m_Views[ i ], m_Projection, nullptr, nullptr );

		m_immediateContext->PSSetShaderResources( 0, 1, &m_textureSRV );

		m_immediateContext->Map( m_perObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped );
		( ( SCBPerObject* ) Mapped.pData )->World = DirectX::XMMatrixTranspose( DirectX::XMMatrixIdentity( ) );
		( ( SCBPerObject* ) Mapped.pData )->View = DirectX::XMMatrixTranspose( m_Views[ i ] );
		( ( SCBPerObject* ) Mapped.pData )->Projection = DirectX::XMMatrixTranspose( m_Projection );
		m_immediateContext->Unmap( m_perObjectBuffer, 0 );
		m_immediateContext->VSSetConstantBuffers( 0, 1, &m_perObjectBuffer );


		m_immediateContext->VSSetShader( m_SkyVertexShader, 0, 0 );
		m_immediateContext->PSSetShader( m_SkyPixelShader, 0, 0 );
		m_immediateContext->IASetInputLayout( m_SkyInputLayout );
		m_immediateContext->OMSetDepthStencilState( m_DSState, 0 );

		m_sky->Render( m_immediateContext );
		m_immediateContext->DrawIndexed( m_sky->GetIndexCount( ), 0, 0 );
		m_immediateContext->OMSetDepthStencilState( 0, 0 );
	}

	m_immediateContext->RSSetViewports( numViewports, &oldViewport );
	m_immediateContext->OMSetRenderTargets( 1, &oldRTV, oldDS );

	// Middle scene sphere
	m_immediateContext->Map( m_perObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped );
	( ( SCBPerObject* ) Mapped.pData )->World = DirectX::XMMatrixTranspose( DirectX::XMMatrixIdentity( ) );
	( ( SCBPerObject* ) Mapped.pData )->View = DirectX::XMMatrixTranspose( View );
	( ( SCBPerObject* ) Mapped.pData )->Projection = DirectX::XMMatrixTranspose( Projection );
	m_immediateContext->Unmap( m_perObjectBuffer, 0 );
	m_immediateContext->VSSetConstantBuffers( 0, 1, &m_perObjectBuffer );

	m_immediateContext->Map( m_perFrameBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped );
	( ( SCBPerFrame* ) Mapped.pData )->CamPos = EyePos;
	m_immediateContext->Unmap( m_perFrameBuffer, 0 );
	m_immediateContext->PSSetConstantBuffers( 0, 1, &m_perFrameBuffer );

	m_immediateContext->PSSetShaderResources( 0, 1, &m_CubemapTexture );

	m_model->Render( m_immediateContext );
	m_immediateContext->VSSetShader( m_vertexShader, 0, 0 );
	m_immediateContext->PSSetShader( m_pixelShader, 0, 0 );
	m_immediateContext->GSSetShader( 0, 0, 0 );
	m_immediateContext->IASetInputLayout( m_inputLayout );

	m_immediateContext->DrawIndexed( m_model->GetIndexCount( ), 0, 0 );

	// Sky
	m_immediateContext->PSSetShaderResources( 0, 1, &m_textureSRV );

	m_immediateContext->Map( m_perObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped );
	( ( SCBPerObject* ) Mapped.pData )->World = DirectX::XMMatrixTranspose( DirectX::XMMatrixTranslation( EyePos.x, EyePos.y, EyePos.z ) );
	( ( SCBPerObject* ) Mapped.pData )->View = DirectX::XMMatrixTranspose( View );
	( ( SCBPerObject* ) Mapped.pData )->Projection = DirectX::XMMatrixTranspose( Projection );
	m_immediateContext->Unmap( m_perObjectBuffer, 0 );
	m_immediateContext->VSSetConstantBuffers( 0, 1, &m_perObjectBuffer );


	m_immediateContext->VSSetShader( m_SkyVertexShader, 0, 0 );
	m_immediateContext->PSSetShader( m_SkyPixelShader, 0, 0 );
	m_immediateContext->IASetInputLayout( m_SkyInputLayout );
	m_immediateContext->OMSetDepthStencilState( m_DSState, 0 );

	m_sky->Render( m_immediateContext );
	m_immediateContext->DrawIndexed( m_sky->GetIndexCount( ), 0, 0 );
	m_immediateContext->OMSetDepthStencilState( 0, 0 );
}

CubeMapping::~CubeMapping( )
{
	SafeRelease( m_vertexShader );
	SafeRelease( m_pixelShader );
	SafeRelease( m_inputLayout );
	SafeRelease( m_perObjectBuffer );
	SafeRelease( m_SkyVertexShader );
	SafeRelease( m_SkyPixelShader );
	SafeRelease( m_SkyInputLayout );
	if ( m_model )
	{
		m_model->Shutdown( );
		delete m_model;
		m_model = 0;
	}
	if ( m_sky )
	{
		m_sky->Shutdown( );
		delete m_sky;
		m_sky = 0;
	}
	if ( m_floor )
	{
		m_floor->Shutdown( );
		delete m_floor;
		m_floor = 0;
	}
}

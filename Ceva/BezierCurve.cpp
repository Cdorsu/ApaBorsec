#include "BezierCurve.h"



BezierCurve::BezierCurve( ID3D11Device * Device, ID3D11DeviceContext * Context,
	BezierCurve::SControlPoints const& Points ) :
	mDevice( Device ),
	mContext( Context )
{
	InitializeBuffers( Points );
	InitializeShaders( );
}


BezierCurve::~BezierCurve( )
{
	for ( auto && iter : mShaders )
	{
		if ( iter )
		{
			iter->Release( );
			iter = nullptr;
		}
	}

	SafeRelease( mVertexShader );
	SafeRelease( mHullShader );
	SafeRelease( mDomainShader );
	SafeRelease( mPixelShader );

	SafeRelease( mVertexBuffer );
	SafeRelease( mPointsBuffer );
	SafeRelease( mPerObjectBuffer );
}

void BezierCurve::Render( DirectX::FXMMATRIX & view, DirectX::FXMMATRIX & projection )
{
	static DirectX::XMMATRIX WVP;
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	static const UINT Strides = sizeof( SVertex );
	static const UINT Offsets = 0;
	WVP = view * projection;

	mContext->VSSetShader( mVertexShader, nullptr, 0 );
	mContext->HSSetShader( mHullShader, nullptr, 0 );
	mContext->DSSetShader( mDomainShader, nullptr, 0 );
	mContext->PSSetShader( mPixelShader, nullptr, 0 );

	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST );
	mContext->IASetVertexBuffers( 0, 1, &mVertexBuffer, &Strides, &Offsets );

	mContext->IASetInputLayout( mLayout );

	mContext->Map( mPerObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( SPerObject* ) MappedSubresource.pData )->WVP = DirectX::XMMatrixTranspose( WVP );
	mContext->Unmap( mPerObjectBuffer, 0 );
	mContext->DSSetConstantBuffers( 0, 1, &mPerObjectBuffer );

	mContext->DSSetConstantBuffers( 1, 1, &mPointsBuffer );

	mContext->Draw( 2, 0 );

	mContext->VSSetShader( nullptr, nullptr, 0 );
	mContext->HSSetShader( nullptr, nullptr, 0 );
	mContext->DSSetShader( nullptr, nullptr, 0 );
	mContext->PSSetShader( nullptr, nullptr, 0 );
}

void BezierCurve::InitializeBuffers( BezierCurve::SControlPoints const& Points )
{
	HRESULT hr;
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, buffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, buffData );

	SVertex Vertices[ 2 ];
	Vertices[ 0 ].Position = Points.ControlPoints[ 0 ];
	Vertices[ 1 ].Position = Points.ControlPoints[ Points.Number - 1 ];
	
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof( SVertex ) * 2;
	buffData.pSysMem = Vertices;

	hr = mDevice->CreateBuffer( &buffDesc, &buffData, &mVertexBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemory( &buffDesc, sizeof( D3D11_BUFFER_DESC ) );
	ZeroMemory( &buffData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( Points );
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffData.pSysMem = &Points;
	hr = mDevice->CreateBuffer( &buffDesc, &buffData, &mPointsBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemory( &buffDesc, sizeof( D3D11_BUFFER_DESC ) );
	ZeroMemory( &buffData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SPerObject );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mPerObjectBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";
}

void BezierCurve::InitializeShaders( )
{
	HRESULT hr;
	ID3DBlob *ShaderBlob;

	hr = D3DX11CompileFromFile( L"BezierCurveVertexShader.hlsl", nullptr, nullptr,
		"main", "vs_5_0", 0, 0, nullptr, &ShaderBlob, nullptr, nullptr );
	if ( FAILED( hr ) )
	{
		throw "Fraier";
	}

	D3D11_INPUT_ELEMENT_DESC layout[ 1 ];
	layout[ 0 ].AlignedByteOffset = 0;
	layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[ 0 ].InputSlot = 0;
	layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 0 ].InstanceDataStepRate = 0;
	layout[ 0 ].SemanticIndex = 0;
	layout[ 0 ].SemanticName = "POSITION";
	UINT numLayout = ARRAYSIZE( layout );
	hr = mDevice->CreateInputLayout(
		layout, numLayout, ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), &mLayout
	);
	if ( FAILED( hr ) )
		throw "Fraier";

	hr = mDevice->CreateVertexShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), nullptr, &mVertexShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	mShaders.push_back( ShaderBlob );

	hr = D3DX11CompileFromFile( L"BezierCurveHullShader.hlsl", nullptr, nullptr,
		"main", "hs_5_0", 0, 0, nullptr, &ShaderBlob, nullptr, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = mDevice->CreateHullShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), nullptr, &mHullShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	mShaders.push_back( ShaderBlob );

	ID3DBlob * ErrorBlob;
	hr = D3DX11CompileFromFile( L"BezierCurveDomainShader.hlsl", nullptr, nullptr,
		"main", "ds_5_0", 0, 0, nullptr, &ShaderBlob, &ErrorBlob, nullptr );
	if ( FAILED( hr ) )
	{
		if ( ErrorBlob )
		{
			OutputDebugStringA( ( char* ) ErrorBlob->GetBufferPointer( ) );
		}
		throw "Fraier";
	}
	hr = mDevice->CreateDomainShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), nullptr, &mDomainShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	mShaders.push_back( ShaderBlob );

	hr = D3DX11CompileFromFile( L"BezierCurvePixelShader.hlsl", nullptr, nullptr,
		"main", "ps_5_0", 0, 0, nullptr, &ShaderBlob, nullptr, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = mDevice->CreatePixelShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), nullptr, &mPixelShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	mShaders.push_back( ShaderBlob );
}
#include "WorldInstancing.h"



WorldInstancing::WorldInstancing( ID3D11Device * device, ID3D11DeviceContext * immediateContext ) :
	m_device( device ),
	m_immediateContext( immediateContext )
{
	InitializeBuffers( );
	InitializeShaders( );
}

WorldInstancing::~WorldInstancing( )
{
	Shutdown( );
}

void WorldInstancing::InitializeBuffers( )
{
	HRESULT hr;

	SVertex Vertices[ ] =
	{
		SVertex( -1.0f,  1.0f, -1.0f ), // 0
		SVertex( 1.0f,  1.0f, -1.0f ), // 1
		SVertex( 1.0f, -1.0f, -1.0f ), // 2
		SVertex( -1.0f, -1.0f, -1.0f ), // 3

		SVertex( -1.0f,  1.0f, 1.0f ), // 4
		SVertex( 1.0f,  1.0f, 1.0f ), // 5
		SVertex( 1.0f, -1.0f, 1.0f ), // 6
		SVertex( -1.0f, -1.0f, 1.0f ), // 7

		SVertex( -1.0f, 1.0f, 1.0f ), // 8
		SVertex( 1.0f, 1.0f, 1.0f ), // 9
		SVertex( 1.0f, 1.0f,-1.0f ), // 10
		SVertex( -1.0f, 1.0f,-1.0f ), // 11

		SVertex( -1.0f, -1.0f, 1.0f ), // 12
		SVertex( 1.0f, -1.0f, 1.0f ), // 13
		SVertex( 1.0f, -1.0f,-1.0f ), // 14
		SVertex( -1.0f, -1.0f,-1.0f ), // 15

		SVertex( -1.0f, 1.0f, 1.0f ), // 16
		SVertex( -1.0f, 1.0f,-1.0f ), // 17
		SVertex( -1.0f,-1.0f,-1.0f ), // 18
		SVertex( -1.0f,-1.0f, 1.0f ), // 19

		SVertex( 1.0f, 1.0f, 1.0f ), // 20
		SVertex( 1.0f, 1.0f,-1.0f ), // 21
		SVertex( 1.0f,-1.0f,-1.0f ), // 22
		SVertex( 1.0f,-1.0f, 1.0f ), // 23
	};
	m_vertexCount = ARRAYSIZE( Vertices );

	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, vertBuffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, vertBuffData );
	vertBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertBuffDesc.ByteWidth = sizeof( SVertex ) * m_vertexCount;
	vertBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	vertBuffData.pSysMem = Vertices;
	hr = m_device->CreateBuffer( &vertBuffDesc, &vertBuffData, &m_vertexBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";


	DWORD Indices[ ] =
	{
		0,  1, 2,
		0,  2, 3,

		4,  5, 6,
		4,  6, 7,

		8,  9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23,
	};
	m_indexCount = ARRAYSIZE( Indices );
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, idxBuffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, idxBuffData );
	idxBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	idxBuffDesc.ByteWidth = sizeof( DWORD ) * m_indexCount;
	idxBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	idxBuffData.pSysMem = Indices;
	hr = m_device->CreateBuffer( &idxBuffDesc, &idxBuffData, &m_indexBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

	using namespace DirectX;
	SInstance Instances[ ] =
	{
		SInstance( XMMatrixTranslation( 5,5,0 ) * XMMatrixRotationZ( 0.34 ),XMFLOAT4( 1,1,0,1 ) ),
		SInstance( XMMatrixIdentity( ),XMFLOAT4( 1,1,1,1 ) ),
		SInstance( XMMatrixTranslation( -5,5,0 ) * XMMatrixRotationZ( -0.34 ), XMFLOAT4(1,0,0,1)),
	};
	m_instanceCount = ARRAYSIZE( Instances );
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, buffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, buffData );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( SInstance ) * m_instanceCount;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffData.pSysMem = Instances;
	hr = m_device->CreateBuffer( &buffDesc, &buffData, &m_instanceBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, cbuffDesc );
	cbuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	cbuffDesc.ByteWidth = sizeof( SConstantBuffer );
	cbuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	cbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = m_device->CreateBuffer( &cbuffDesc, nullptr, &m_constantBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

}

void WorldInstancing::InitializeShaders( )
{
	HRESULT hr;

	ID3DBlob *Shader, *Errors;
	hr = D3DX11CompileFromFile( L"WorldInstancingVertexShader.hlsl", nullptr, nullptr,
								"main", "vs_4_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = m_device->CreateVertexShader( Shader->GetBufferPointer( ),
		Shader->GetBufferSize( ),
		nullptr, &m_vertexShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	D3D11_INPUT_ELEMENT_DESC layout[ 6 ];
	layout[ 0 ].AlignedByteOffset = 0;
	layout[ 0 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 0 ].InputSlot = 0;
	layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 0 ].InstanceDataStepRate = 0;
	layout[ 0 ].SemanticIndex = 0;
	layout[ 0 ].SemanticName = "POSITION";

	layout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 1 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[ 1 ].InputSlot = 1;
	layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[ 1 ].InstanceDataStepRate = 1;
	layout[ 1 ].SemanticIndex = 0;
	layout[ 1 ].SemanticName = "WORLD";
	layout[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 2 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[ 2 ].InputSlot = 1;
	layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[ 2 ].InstanceDataStepRate = 1;
	layout[ 2 ].SemanticIndex = 1;
	layout[ 2 ].SemanticName = "WORLD";
	layout[ 3 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 3 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[ 3 ].InputSlot = 1;
	layout[ 3 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[ 3 ].InstanceDataStepRate = 1;
	layout[ 3 ].SemanticIndex = 2;
	layout[ 3 ].SemanticName = "WORLD";
	layout[ 4 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 4 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[ 4 ].InputSlot = 1;
	layout[ 4 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[ 4 ].InstanceDataStepRate = 1;
	layout[ 4 ].SemanticIndex = 3;
	layout[ 4 ].SemanticName = "WORLD";

	layout[ 5 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 5 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[ 5 ].InputSlot = 1;
	layout[ 5 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[ 5 ].InstanceDataStepRate = 1;
	layout[ 5 ].SemanticIndex = 0;
	layout[ 5 ].SemanticName = "COLOR";

	hr = m_device->CreateInputLayout(
		layout, sizeof( layout ) / sizeof( layout[ 0 ] ),
		Shader->GetBufferPointer( ), Shader->GetBufferSize( ),
		&m_inputLayout
		);
	if ( FAILED( hr ) )
		throw "Fraier";

	SafeRelease( Shader );
	SafeRelease( Errors );
	hr = D3DX11CompileFromFile( L"WorldInstancingPixelShader.hlsl", nullptr, nullptr,
		"main", "ps_4_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";

	hr = m_device->CreatePixelShader( Shader->GetBufferPointer( ),
		Shader->GetBufferSize( ), nullptr, &m_pixelShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	SafeRelease( Shader );
	SafeRelease( Errors );
}

void WorldInstancing::UpdateBuffers( DirectX::FXMMATRIX& v, DirectX::FXMMATRIX& p )
{
	static D3D11_MAPPED_SUBRESOURCE MappedResource;
	static HRESULT hr;
	static DirectX::XMMATRIX VP;
	VP = v * p;
	m_immediateContext->Map( m_constantBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	( ( SConstantBuffer* ) MappedResource.pData )->VP = DirectX::XMMatrixTranspose( VP );
	m_immediateContext->Unmap( m_constantBuffer, 0 );
	m_immediateContext->VSSetConstantBuffers( 0, 1, &m_constantBuffer );
}

void WorldInstancing::Render( DirectX::FXMMATRIX& viewMatrix, DirectX::FXMMATRIX& projectionMatrix )
{
	UINT Strides[ 2 ] = { sizeof( SVertex ), sizeof( SInstance ) };
	UINT Offsets[ 2 ] = { 0,0 };
	
	ID3D11Buffer *vbs[ 2 ] = { m_vertexBuffer,m_instanceBuffer };
	m_immediateContext->IASetVertexBuffers( 0, 2, vbs, Strides, Offsets );
	m_immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_immediateContext->IASetInputLayout( m_inputLayout );
	m_immediateContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	
	m_immediateContext->VSSetShader( m_vertexShader, 0, 0 );
	m_immediateContext->PSSetShader( m_pixelShader, 0, 0 );
	UpdateBuffers( viewMatrix, projectionMatrix );
	m_immediateContext->DrawIndexedInstanced( m_indexCount, m_instanceCount, 0, 0, 0 );
}

void WorldInstancing::Shutdown( )
{
	SafeRelease( m_indexBuffer );
	SafeRelease( m_vertexBuffer );
	SafeRelease( m_instanceBuffer );
	SafeRelease( m_constantBuffer );
	SafeRelease( m_vertexShader );
	SafeRelease( m_inputLayout );
	SafeRelease( m_pixelShader );
}

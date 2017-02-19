#include "InstanceShader.h"



CInstanceShader::CInstanceShader( )
{
	ZeroMemory( this, sizeof( CInstanceShader ) );
}

bool CInstanceShader::Initialize( ID3D11Device * device )
{
	ID3D10Blob *ShaderBlob, *ErrorBlob;
	HRESULT hr;
	hr = D3DX11CompileFromFile( L"InstancedVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
	if ( FAILED( hr ) )
	{
		if ( ShaderBlob )
			OutputShaderError( ShaderBlob );
		return false;
	}
	hr = device->CreateVertexShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), NULL, &VertexShader );
	if ( FAILED( hr ) )
		return false;
	D3D11_INPUT_ELEMENT_DESC layout[ 3 ];
	layout[ 0 ].AlignedByteOffset = 0;
	layout[ 0 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 0 ].InputSlot = 0;
	layout[ 0 ].InstanceDataStepRate = 0;
	layout[ 0 ].SemanticIndex = 0;
	layout[ 0 ].SemanticName = "POSITION";
	layout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 1 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[ 1 ].InputSlot = 1;
	layout[ 1 ].InstanceDataStepRate = 1;
	layout[ 1 ].SemanticIndex = 0;
	layout[ 1 ].SemanticName = "INSTANCEPOS";
	layout[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 2 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[ 2 ].InputSlot = 1;
	layout[ 2 ].InstanceDataStepRate = 1;
	layout[ 2 ].SemanticIndex = 0;
	layout[ 2 ].SemanticName = "INSTANCECOLOR";
	hr = device->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[ 0 ] ),
		ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), &InputLayout );
	if ( FAILED( hr ) )
		return false;
	SafeRelease( ShaderBlob );
	hr = D3DX11CompileFromFile( L"InstancedPixelShader.hlsl", NULL, NULL, "main", "ps_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
	if ( FAILED( hr ) )
	{
		if ( ShaderBlob )
			OutputShaderError( ErrorBlob );
		return false;
	}
	hr = device->CreatePixelShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), NULL, &PixelShader );
	if ( FAILED( hr ) )
		return false;

	D3D11_BUFFER_DESC buffDesc = { 0 };
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SConstantBuffer );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer( &buffDesc, NULL, &ConstantObjectBuffer );
	if ( FAILED( hr ) )
		return false;

	return true;
}

void CInstanceShader::Render( ID3D11DeviceContext * context, UINT IndexCount, UINT InstanceCount,
	DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection )
{
	using namespace DirectX;
	static HRESULT hr;
	static XMMATRIX WVP;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	context->IASetInputLayout( InputLayout );
	context->VSSetShader( VertexShader, nullptr, 0 );
	context->PSSetShader( PixelShader, nullptr, 0 );
	WVP = World * View * Projection;
	context->Map( ConstantObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	( ( SConstantBuffer* ) MappedResource.pData )->WVP = DirectX::XMMatrixTranspose( WVP );
	context->Unmap( ConstantObjectBuffer, 0 );
	context->VSSetConstantBuffers( 0, 1, &ConstantObjectBuffer );
	context->DrawIndexedInstanced( IndexCount, InstanceCount, 0, 0, 0 );
}

void CInstanceShader::OutputShaderError( ID3D10Blob * Error )
{
	std::ofstream ofs( "ShaderError.txt" );
	size_t Size = Error->GetBufferSize( );
	char *error = ( char* ) Error->GetBufferPointer( );
	for ( size_t i = 0; i < Size; ++i )
		ofs << error[ i ];
	ofs.close( );
}

void CInstanceShader::Shutdown( )
{
	SafeRelease( VertexShader );
	SafeRelease( PixelShader );
	SafeRelease( InputLayout );
	SafeRelease( ConstantObjectBuffer );
}

CInstanceShader::~CInstanceShader( )
{
	Shutdown( );
}

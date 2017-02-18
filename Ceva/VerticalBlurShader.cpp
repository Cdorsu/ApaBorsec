#include "VerticalBlurShader.h"



CVerticalBlurShader::CVerticalBlurShader( )
{
	ZeroMemory( this, sizeof( CVerticalBlurShader ) );
}

bool CVerticalBlurShader::Initialize( ID3D11Device * device )
{
	ID3D10Blob *ShaderBlob, *ErrorBlob;
	HRESULT hr;
	hr = D3DX11CompileFromFile( L"VerticalBlurVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
	if ( FAILED( hr ) )
	{
		if ( ShaderBlob )
			OutputShaderError( ShaderBlob );
		return false;
	}
	hr = device->CreateVertexShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), NULL, &VertexShader );
	if ( FAILED( hr ) )
		return false;
	D3D11_INPUT_ELEMENT_DESC layout[ 2 ];
	layout[ 0 ].AlignedByteOffset = 0;
	layout[ 0 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 0 ].InputSlot = 0;
	layout[ 0 ].InstanceDataStepRate = 0;
	layout[ 0 ].SemanticIndex = 0;
	layout[ 0 ].SemanticName = "POSITION";
	layout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 1 ].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 1 ].InputSlot = 0;
	layout[ 1 ].InstanceDataStepRate = 0;
	layout[ 1 ].SemanticIndex = 0;
	layout[ 1 ].SemanticName = "TEXCOORD";
	hr = device->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[ 0 ] ),
		ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), &InputLayout );
	if ( FAILED( hr ) )
		return false;
	SafeRelease( ShaderBlob );
	hr = D3DX11CompileFromFile( L"BlurPixelShader.hlsl", NULL, NULL, "main", "ps_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
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
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SConstantBuffer );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer( &buffDesc, NULL, &ConstantBuffer );
	if ( FAILED( hr ) )
		return false;
	buffDesc.ByteWidth = sizeof( SWindowInfo );
	hr = device->CreateBuffer( &buffDesc, NULL, &WindowBuffer );
	if ( FAILED( hr ) )
		return false;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof( sampDesc ) );
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MipLODBias = 3;
	hr = device->CreateSamplerState( &sampDesc, &Sampler );
	if ( FAILED( hr ) )
		return false;


	return true;
}

void CVerticalBlurShader::Render( ID3D11DeviceContext * context, UINT IndexDrawAmount, ID3D11ShaderResourceView * ShaderResource,
	DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection, float windowHeight )
{
	using namespace DirectX;
	static HRESULT hr;
	static XMMATRIX WVP;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	context->IASetInputLayout( InputLayout );
	context->VSSetShader( VertexShader, nullptr, 0 );
	context->PSSetShader( PixelShader, nullptr, 0 );
	WVP = World * View * Projection;
	hr = context->Map( ConstantBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SConstantBuffer* ) MappedResource.pData )->WVP = XMMatrixTranspose( WVP );
	context->Unmap( ConstantBuffer, 0 );
	context->VSSetConstantBuffers( 0, 1, &ConstantBuffer );
	hr = context->Map( WindowBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SWindowInfo* ) MappedResource.pData )->windowHeight = windowHeight;
	context->Unmap( WindowBuffer, 0 );
	context->VSSetConstantBuffers( 1, 1, &WindowBuffer );
	context->PSSetSamplers( 0, 1, &Sampler );
	context->PSSetShaderResources( 0, 1, &ShaderResource );
	context->DrawIndexed( IndexDrawAmount, 0, 0 );
}

void CVerticalBlurShader::OutputShaderError( ID3D10Blob * Error )
{
	std::ofstream ofs( "ShaderError.txt" );
	size_t Size = Error->GetBufferSize( );
	char *error = ( char* ) Error->GetBufferPointer( );
	for ( size_t i = 0; i < Size; ++i )
		ofs << error[ i ];
	ofs.close( );
}

void CVerticalBlurShader::Shutdown( )
{
	SafeRelease( VertexShader );
	SafeRelease( PixelShader );
	SafeRelease( InputLayout );
	SafeRelease( WindowBuffer );
	SafeRelease( ConstantBuffer );
	SafeRelease( Sampler );
}

CVerticalBlurShader::~CVerticalBlurShader( )
{
	Shutdown( );
}

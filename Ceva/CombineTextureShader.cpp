#include "CombineTextureShader.h"



CCombineTextureShader::CCombineTextureShader( )
{
	ZeroMemory( this, sizeof( CCombineTextureShader ) );
}

bool CCombineTextureShader::Initialize( ID3D11Device * device )
{
	ID3D10Blob *ShaderBlob, *ErrorBlob;
	HRESULT hr;
	hr = D3DX11CompileFromFile( L"CombineTexturesVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
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
	layout[ 1 ].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 1 ].InputSlot = 0;
	layout[ 1 ].InstanceDataStepRate = 0;
	layout[ 1 ].SemanticIndex = 0;
	layout[ 1 ].SemanticName = "TEXCOORD";
	layout[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 2 ].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 2 ].InputSlot = 0;
	layout[ 2 ].InstanceDataStepRate = 0;
	layout[ 2 ].SemanticIndex = 1;
	layout[ 2 ].SemanticName = "TEXCOORD";
	hr = device->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[ 0 ] ),
		ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), &InputLayout );
	if ( FAILED( hr ) )
		return false;
	SafeRelease( ShaderBlob );
	hr = D3DX11CompileFromFile( L"CombineTexturesPixelShader.hlsl", NULL, NULL, "main", "ps_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
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
	buffDesc.ByteWidth = sizeof( SGlowInfo );
	hr = device->CreateBuffer( &buffDesc, NULL, &GlowInfoBuffer );
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
	hr = device->CreateSamplerState( &sampDesc, &WrapSampler );
	if ( FAILED( hr ) )
		return false;


	return true;
}

void CCombineTextureShader::Render( ID3D11DeviceContext * context, UINT IndexDrawAmount, ID3D11ShaderResourceView * texture,
	ID3D11ShaderResourceView * glow, DirectX::XMMATRIX& World, DirectX::XMMATRIX& View,
	DirectX::XMMATRIX& Projection, float toAdd, float glowStrength )
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
	( ( SConstantBuffer* ) MappedResource.pData )->ToAdd = toAdd;
	context->Unmap( ConstantBuffer, 0 );
	context->VSSetConstantBuffers( 0, 1, &ConstantBuffer );

	hr = context->Map( GlowInfoBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SGlowInfo* ) MappedResource.pData )->glowStrength = glowStrength;
	context->Unmap( GlowInfoBuffer, 0 );
	context->PSSetConstantBuffers( 0, 1, &GlowInfoBuffer );

	context->PSSetSamplers( 0, 1, &WrapSampler );
	context->PSSetShaderResources( 0, 1, &texture );
	context->PSSetShaderResources( 1, 1, &glow );
	context->DrawIndexed( IndexDrawAmount, 0, 0 );
}

void CCombineTextureShader::OutputShaderError( ID3D10Blob * Error )
{
	std::ofstream ofs( "ShaderError.txt" );
	size_t Size = Error->GetBufferSize( );
	char *error = ( char* ) Error->GetBufferPointer( );
	for ( size_t i = 0; i < Size; ++i )
		ofs << error[ i ];
	ofs.close( );
}

void CCombineTextureShader::Shutdown( )
{
	SafeRelease( VertexShader );
	SafeRelease( PixelShader );
	SafeRelease( InputLayout );
	SafeRelease( ConstantBuffer );
	SafeRelease( GlowInfoBuffer );
	SafeRelease( WrapSampler );
}

CCombineTextureShader::~CCombineTextureShader( )
{
	Shutdown( );
}

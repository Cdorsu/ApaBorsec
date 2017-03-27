#include "ShadowShader.h"



CShadowShader::CShadowShader( )
{
	ZeroMemory( this, sizeof( CShadowShader ) );
}

bool CShadowShader::Initialize( ID3D11Device * device )
{
	ID3D10Blob *ShaderBlob = nullptr, *ErrorBlob = nullptr;
	HRESULT hr = S_OK;
	hr = D3DX11CompileFromFile( L"ShadowVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
	if ( FAILED( hr ) )
	{
		if ( ErrorBlob )
			OutputShaderError( ErrorBlob );
		return false;
	}
	/*hr = D3DReadFileToBlob( L"ShadowVertexShader.cso", &ShaderBlob );
	if ( FAILED( hr ) )
		return false;*/
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
	layout[ 2 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 2 ].InputSlot = 0;
	layout[ 2 ].InstanceDataStepRate = 0;
	layout[ 2 ].SemanticIndex = 0;
	layout[ 2 ].SemanticName = "NORMAL";
	hr = device->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[ 0 ] ),
		ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), &InputLayout );
	if ( FAILED( hr ) )
		return false;
	SafeRelease( ShaderBlob );
	hr = D3DX11CompileFromFile( L"ShadowPixelShader.hlsl", NULL, NULL, "main", "ps_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
	if ( FAILED( hr ) )
	{
		if ( ErrorBlob )
			OutputShaderError( ErrorBlob );
		return false;
	}
	/*hr = D3DReadFileToBlob( L"ShadowPixelShader.cso", &ShaderBlob );
	if ( FAILED( hr ) )
		return false;*/
	hr = device->CreatePixelShader( ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), NULL, &PixelShader );
	if ( FAILED( hr ) )
		return false;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof( sampDesc ) );
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 0;
	sampDesc.MipLODBias = 3;
	hr = device->CreateSamplerState( &sampDesc, &ClampSampler );
	if ( FAILED( hr ) )
		return false;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	hr = device->CreateSamplerState( &sampDesc, &WrapSampler );
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
	buffDesc.ByteWidth = sizeof( SLightVS );
	hr = device->CreateBuffer( &buffDesc, NULL, &VSLightBuffer );
	if ( FAILED( hr ) )
		return false;
	buffDesc.ByteWidth = sizeof( SLightPS );
	hr = device->CreateBuffer( &buffDesc, NULL, &PSLightBuffer );
	if ( FAILED( hr ) )
		return false;

	return true;
}

void CShadowShader::Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
	ID3D11ShaderResourceView * Texture, ID3D11ShaderResourceView * Depthmap,
	DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
	CLightView* Light )
{
	using namespace DirectX;
	static HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	context->IASetInputLayout( InputLayout );
	context->VSSetShader( VertexShader, NULL, 0 );
	context->PSSetShader( PixelShader, NULL, 0 );

	hr = context->Map( ConstantBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SConstantBuffer* ) MappedResource.pData )->World = XMMatrixTranspose( World );
	( ( SConstantBuffer* ) MappedResource.pData )->View = XMMatrixTranspose( View );
	( ( SConstantBuffer* ) MappedResource.pData )->Projection = XMMatrixTranspose( Projection );
	( ( SConstantBuffer* ) MappedResource.pData )->LightView = XMMatrixTranspose( Light->GetView( ) );
	( ( SConstantBuffer* ) MappedResource.pData )->LightProjection = XMMatrixTranspose( Light->GetProjection( ) );
	context->Unmap( ConstantBuffer, 0 );
	context->VSSetConstantBuffers( 0, 1, &ConstantBuffer );

	hr = context->Map( VSLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SLightVS* ) MappedResource.pData )->Pos = Light->GetPosition( );
	context->Unmap( VSLightBuffer, 0 );
	context->VSSetConstantBuffers( 1, 1, &VSLightBuffer );

	hr = context->Map( PSLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SLightPS* ) MappedResource.pData )->Diffuse = Light->GetDiffuse( );
	( ( SLightPS* ) MappedResource.pData )->Ambient = Light->GetAmbient( );
	context->Unmap( PSLightBuffer, 0 );
	context->PSSetConstantBuffers( 0, 1, &PSLightBuffer );

	context->PSSetSamplers( 0, 1, &WrapSampler );
	context->PSSetSamplers( 1, 1, &ClampSampler );

	context->PSSetShaderResources( 0, 1, &Texture );
	context->PSSetShaderResources( 1, 1, &Depthmap );


	context->DrawIndexed( IndexDrawAmount, 0, 0 );

}

void CShadowShader::OutputShaderError( ID3D10Blob * Error )
{
	std::ofstream ofs( "ShaderError.txt" );
	UINT Size = ( UINT ) Error->GetBufferSize( );
	char *error = ( char* ) Error->GetBufferPointer( );
	for ( UINT i = 0; i < Size; ++i )
		ofs << error[ i ];
	ofs.close( );
}

void CShadowShader::Shutdown( )
{
	SafeRelease( VertexShader );
	SafeRelease( PixelShader );
	SafeRelease( InputLayout );
	SafeRelease( ClampSampler );
	SafeRelease( WrapSampler );
	SafeRelease( ConstantBuffer );
	SafeRelease( VSLightBuffer );
	SafeRelease( PSLightBuffer );
}

CShadowShader::~CShadowShader( )
{
	Shutdown( );
}

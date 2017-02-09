#include "TextureShader.h"



CTextureShader::CTextureShader( )
{
	ZeroMemory( this, sizeof( CTextureShader ) );
}

bool CTextureShader::Initialize( ID3D11Device * device )
{
	ID3D10Blob *ShaderBlob = nullptr, *ErrorBlob = nullptr;
	HRESULT hr = S_OK;
	hr = D3DX11CompileFromFile( L"TextureVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
	if ( FAILED( hr ) )
	{
		if ( ShaderBlob )
			OutputShaderError( ShaderBlob );
		return false;
	}
	//hr = D3DReadFileToBlob( L"VertexShader.cso", &ShaderBlob );
	//if (FAILED( hr ))
	//return false;
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
	hr = D3DX11CompileFromFile( L"TexturePixelShader.hlsl", NULL, NULL, "main", "ps_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
	if ( FAILED( hr ) )
	{
		if ( ShaderBlob )
			OutputShaderError( ErrorBlob );
		return false;
	}
	//hr = D3DReadFileToBlob( L"PixelShader.cso", &ShaderBlob );
	//if (FAILED( hr ))
	//return false;
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
	buffDesc.ByteWidth = sizeof( SLight );
	hr = device->CreateBuffer( &buffDesc, NULL, &LightBuffer );
	if ( FAILED( hr ) )
		return false;
	buffDesc.ByteWidth = sizeof( SPointLight );
	hr = device->CreateBuffer( &buffDesc, NULL, &PointLightBuffer );
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

void CTextureShader::Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
	ID3D11ShaderResourceView * Texture,
	DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
	CLight *light, CPointLight *Pointlight, DirectX::XMFLOAT4 clipPlane )
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
	( ( SConstantBuffer* ) MappedResource.pData )->World = XMMatrixTranspose( World );
	( ( SConstantBuffer* ) MappedResource.pData )->ClipPlane = clipPlane;
	context->Unmap( ConstantBuffer, 0 );
	context->VSSetConstantBuffers( 0, 1, &ConstantBuffer );
	hr = context->Map( LightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SLight* ) MappedResource.pData )->Dir = light->GetDirection( );
	( ( SLight* ) MappedResource.pData )->Ambient = light->GetAmbientColor( );
	( ( SLight* ) MappedResource.pData )->Diffuse = light->GetDiffuseColor( );
	context->Unmap( LightBuffer, 0 );
	context->PSSetConstantBuffers( 0, 1, &LightBuffer );
	hr = context->Map( PointLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if ( FAILED( hr ) )
		return;
	( ( SPointLight* ) MappedResource.pData )->Attenuation = Pointlight->GetAttenuation( );
	( ( SPointLight* ) MappedResource.pData )->Diffuse = Pointlight->GetDiffuseColor( );
	( ( SPointLight* ) MappedResource.pData )->Position = Pointlight->GetPosition( );
	( ( SPointLight* ) MappedResource.pData )->Range = Pointlight->GetRange( );
	context->Unmap( PointLightBuffer, 0 );
	context->PSSetConstantBuffers( 1, 1, &PointLightBuffer );
	context->PSSetSamplers( 0, 1, &Sampler );
	context->PSSetShaderResources( 0, 1, &Texture );
	context->DrawIndexed( IndexDrawAmount, 0, 0 );
}

void CTextureShader::OutputShaderError( ID3D10Blob * Error )
{
	std::ofstream ofs( "ShaderError.txt" );
	UINT Size = ( UINT ) Error->GetBufferSize( );
	char *error = ( char* ) Error->GetBufferPointer( );
	for ( UINT i = 0; i < Size; ++i )
		ofs << error[ i ];
	ofs.close( );
}

void CTextureShader::Shutdown( )
{
	SafeRelease( VertexShader );
	SafeRelease( PixelShader );
	SafeRelease( InputLayout );
	SafeRelease( ConstantBuffer );
	SafeRelease( LightBuffer );
	SafeRelease( Sampler );
}

CTextureShader::~CTextureShader( )
{
	Shutdown( );
}

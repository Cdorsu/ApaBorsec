#include "SimpleShader.h"



CSimpleShader::CSimpleShader( )
{
	ZeroMemory( this, sizeof( CSimpleShader ) );
}

bool CSimpleShader::Initialize( ID3D11Device * device, CSimpleShader::EType type )
{
	ShaderType = type;
	ID3D10Blob *ShaderBlob = nullptr, *ErrorBlob = nullptr;
	HRESULT hr = S_OK;
	switch ( type )
	{
	case CSimpleShader::EType::PlaneClipping:
		hr = D3DX11CompileFromFile( L"PlaneClippingVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
		break;
	case CSimpleShader::EType::NoPlaneClipping:
		hr = D3DX11CompileFromFile( L"VertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
		break;
	default:
		break;
	}
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
	D3D11_INPUT_ELEMENT_DESC layout[ 5 ];
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
	layout[ 3 ].AlignedByteOffset = 0;
	layout[ 3 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 3 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 3 ].InputSlot = 0;
	layout[ 3 ].InstanceDataStepRate = 0;
	layout[ 3 ].SemanticIndex = 0;
	layout[ 3 ].SemanticName = "BINORMAL";
	layout[ 4 ].AlignedByteOffset = 0;
	layout[ 4 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 4 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 4 ].InputSlot = 0;
	layout[ 4 ].InstanceDataStepRate = 0;
	layout[ 4 ].SemanticIndex = 0;
	layout[ 4 ].SemanticName = "TANGENT";
	hr = device->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[ 0 ] ),
		ShaderBlob->GetBufferPointer( ), ShaderBlob->GetBufferSize( ), &InputLayout );
	if ( FAILED( hr ) )
		return false;
	SafeRelease( ShaderBlob );
	hr = D3DX11CompileFromFile( L"PixelShader.hlsl", NULL, NULL, "main", "ps_4_0", NULL, NULL, NULL, &ShaderBlob, &ErrorBlob, NULL );
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
	//buffDesc.ByteWidth = sizeof (SLight); /// For some God damn reason, it doesn't work on laptop
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SLight );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffDesc.CPUAccessFlags = 0;
	hr = device->CreateBuffer( &buffDesc, NULL, &LightBuffer );
	if ( FAILED( hr ) )
		return false;
	if ( ShaderType == EType::PlaneClipping )
	{
		buffDesc.ByteWidth = sizeof( SPlane );
		hr = device->CreateBuffer( &buffDesc, NULL, &PlaneBuffer );
		if ( FAILED( hr ) )
			return false;
	}
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

void CSimpleShader::Render( ID3D11DeviceContext * context, UINT IndexDrawAmount,
	ID3D11ShaderResourceView * Texture, ID3D11ShaderResourceView * BumpMap, ID3D11ShaderResourceView* Specular,
	DirectX::XMMATRIX& World, DirectX::XMMATRIX& View, DirectX::XMMATRIX& Projection,
	DirectX::XMFLOAT3 CameraPos, CLight* light, DirectX::XMFLOAT4 ClippingPlane )
{
	using namespace DirectX;
	static HRESULT hr;
	static XMMATRIX WVP;
	static SLight Light;
	static SPlane Plane;
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
	( ( SConstantBuffer* ) MappedResource.pData )->CamPos = CameraPos;
	context->Unmap( ConstantBuffer, 0 );
	context->VSSetConstantBuffers( 0, 1, &ConstantBuffer );
	if ( ShaderType == EType::PlaneClipping )
	{
		Plane.Plane = ClippingPlane;
		context->UpdateSubresource( PlaneBuffer, 0, NULL, &Plane, 0, 0 );
		context->VSSetConstantBuffers( 1, 1, &PlaneBuffer );
	}
	/*hr = context->Map( LightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	if (FAILED( hr ))
	return;
	((SLight*)MappedResource.pData)->SpecularPower = SpecularPower;
	((SLight*)MappedResource.pData)->SpecularColor = SpecularColor;
	((SLight*)MappedResource.pData)->LightDir = LightDir;
	((SLight*)MappedResource.pData)->AmbientColor = AmbientColor;
	((SLight*)MappedResource.pData)->DiffuseColor = DiffuseColor;
	context->Unmap( ConstantBuffer, 0 );*/ /// FOr some God damn reason, map/unmap doesn't work on laptop
	Light.AmbientColor = light->GetAmbientColor( );
	Light.DiffuseColor = light->GetDiffuseColor( );
	Light.LightDir = light->GetDirection( );
	Light.SpecularColor = light->GetSpecularColor( );
	Light.SpecularPower = light->GetSpecularPower( );
	context->UpdateSubresource( LightBuffer, 0, NULL, &Light, 0, 0 );
	context->PSSetConstantBuffers( 0, 1, &LightBuffer );
	context->PSSetSamplers( 0, 1, &Sampler );
	context->PSSetShaderResources( 0, 1, &Texture );
	context->PSSetShaderResources( 1, 1, &BumpMap );
	context->PSSetShaderResources( 2, 1, &Specular );
	context->DrawIndexed( IndexDrawAmount, 0, 0 );
}

void CSimpleShader::OutputShaderError( ID3D10Blob * Error )
{
	std::ofstream ofs( "ShaderError.txt" );
	UINT Size = ( UINT ) Error->GetBufferSize( );
	char *error = ( char* ) Error->GetBufferPointer( );
	for ( UINT i = 0; i < Size; ++i )
		ofs << error[ i ];
	ofs.close( );
}

void CSimpleShader::Shutdown( )
{
	SafeRelease( VertexShader );
	SafeRelease( PixelShader );
	SafeRelease( InputLayout );
	SafeRelease( ConstantBuffer );
	SafeRelease( LightBuffer );
	SafeRelease( PlaneBuffer );
	SafeRelease( Sampler );
}

CSimpleShader::~CSimpleShader( )
{
	Shutdown( );
}

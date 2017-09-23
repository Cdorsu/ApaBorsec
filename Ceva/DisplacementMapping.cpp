#include "DisplacementMapping.h"



DisplacementMapping::DisplacementMapping( ID3D11Device* Device, ID3D11DeviceContext * Context ):
	mDevice( Device ),
	mContext( Context )
{
	HRESULT hr;
	ID3DBlob *Shader, *Errors;
	hr = D3DX11CompileFromFile( L"DisplacementMappingVertexShader.hlsl", nullptr, nullptr,
		"main", "vs_5_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
	{
		throw "Fraier";
	}
	hr = mDevice->CreateVertexShader( Shader->GetBufferPointer( ), Shader->GetBufferSize( ), nullptr, &mVertexShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	D3D11_INPUT_ELEMENT_DESC layout[ 5 ];
	layout[ 0 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
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
	layout[ 3 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 3 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 3 ].InputSlot = 0;
	layout[ 3 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 3 ].InstanceDataStepRate = 0;
	layout[ 3 ].SemanticIndex = 0;
	layout[ 3 ].SemanticName = "TANGENT";
	layout[ 4 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 4 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 4 ].InputSlot = 0;
	layout[ 4 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 4 ].InstanceDataStepRate = 0;
	layout[ 4 ].SemanticIndex = 0;
	layout[ 4 ].SemanticName = "BINORMAL";
	hr = mDevice->CreateInputLayout( layout, ARRAYSIZE( layout ),
		Shader->GetBufferPointer( ), Shader->GetBufferSize( ), &mInputLayout );
	if ( FAILED( hr ) )
		throw "Fraier";


	SafeRelease( Shader );
	SafeRelease( Errors );
	hr = D3DX11CompileFromFile( L"DisplacementMappingHullShader.hlsl", nullptr, nullptr,
		"main", "hs_5_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
	{
		throw "Fraier";
	}
	hr = mDevice->CreateHullShader( Shader->GetBufferPointer( ), Shader->GetBufferSize( ), nullptr, &mHullShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	SafeRelease( Shader );
	SafeRelease( Errors );
	hr = D3DX11CompileFromFile( L"DisplacementMappingDomainShader.hlsl", nullptr, nullptr,
		"main", "ds_5_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
	{
		throw "Fraier";
	}
	hr = mDevice->CreateDomainShader( Shader->GetBufferPointer( ), Shader->GetBufferSize( ), nullptr, &mDomainShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	SafeRelease( Shader );
	SafeRelease( Errors );
	hr = D3DX11CompileFromFile( L"DisplacementMappingPixelShader.hlsl", nullptr, nullptr,
		"main", "ps_5_0", 0, 0, nullptr, &Shader, &Errors, nullptr );
	if ( FAILED( hr ) )
	{
		throw "Fraier";
	}
	hr = mDevice->CreatePixelShader( Shader->GetBufferPointer( ), Shader->GetBufferSize( ), nullptr, &mPixelShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	SafeRelease( Shader );
	SafeRelease( Errors );


	mModel = new CModel( );
	if ( !mModel->Initialize( mDevice, L"Data/sphere.txt", L"Data/bricks.dds", L"Data/bricks_nmap.dds" ) )
		throw "Fraier";

	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, buffDesc );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( VSBuffer );
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mVSBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mDSBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemoryAndDeclare( D3D11_SAMPLER_DESC, sampDesc );
	sampDesc.AddressU =
		sampDesc.AddressV =
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MipLODBias = 3;
	hr = mDevice->CreateSamplerState( &sampDesc, &mWrapSampler );
	if ( FAILED( hr ) )
		throw "Fraier";

	ZeroMemory( &buffDesc, sizeof( decltype( buffDesc ) ) );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( PSBuffer );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mPSBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";


}


DisplacementMapping::~DisplacementMapping( )
{
	SafeRelease( mVertexShader );
	SafeRelease( mHullShader );
	SafeRelease( mDomainShader );
	SafeRelease( mPixelShader );

	SafeRelease( mInputLayout );

	SafeRelease( mVSBuffer );
	SafeRelease( mDSBuffer );
	SafeRelease( mPSBuffer );
	SafeRelease( mWrapSampler );

	if ( mModel )
	{
		mModel->Shutdown( );
		delete mModel;
		mModel = 0;
	}
}

void DisplacementMapping::Render( DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection, DirectX::XMFLOAT3 EyePos, CLight * Light )
{

	mModel->RotateY( 0.002f );

	static DirectX::XMMATRIX Matrix;
	static D3D11_MAPPED_SUBRESOURCE Mapped;

	mModel->Render( mContext );
	mContext->IASetInputLayout( mInputLayout );

	mContext->VSSetShader( mVertexShader, 0, 0 );
	mContext->HSSetShader( mHullShader, 0, 0 );
	mContext->DSSetShader( mDomainShader, 0, 0 );
	mContext->PSSetShader( mPixelShader, 0, 0 );

	mContext->DSSetSamplers( 0, 1, &mWrapSampler );
	mContext->PSSetSamplers( 0, 1, &mWrapSampler );

	mContext->Map( mVSBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped );

	( ( VSBuffer* ) Mapped.pData )->World = DirectX::XMMatrixTranspose( mModel->GetWorld( ) );
	( ( VSBuffer* ) Mapped.pData )->EyePos = EyePos;

	mContext->Unmap( mVSBuffer, 0 );
	mContext->VSSetConstantBuffers( 0, 1, &mVSBuffer );

	mContext->Map( mDSBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped );

	( ( DSBuffer* ) Mapped.pData )->ViewProjection = DirectX::XMMatrixTranspose( View * Projection );
	( ( DSBuffer* ) Mapped.pData )->EyePos = EyePos;

	mContext->Unmap( mDSBuffer, 0 );
	mContext->DSSetConstantBuffers( 0, 1, &mDSBuffer );

	mContext->Map( mPSBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped );

	( ( PSBuffer* ) Mapped.pData )->Ambient = Light->GetAmbientColor( );
	( ( PSBuffer* ) Mapped.pData )->Diffuse = Light->GetDiffuseColor( );
	( ( PSBuffer* ) Mapped.pData )->LightDir = Light->GetDirection( );

	mContext->Unmap( mPSBuffer, 0 );
	mContext->PSSetConstantBuffers( 0, 1, &mPSBuffer );

	ID3D11ShaderResourceView* SRV = mModel->GetBumpMap( );
	mContext->DSSetShaderResources( 0, 1, &SRV );
	mContext->PSSetShaderResources( 1, 1, &SRV );
	SRV = mModel->GetTexture( );
	mContext->PSSetShaderResources( 0, 1, &SRV );

	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );

	mContext->DrawIndexed( mModel->GetIndexCount( ), 0, 0 );

	mContext->HSSetShader( 0, 0, 0 );
	mContext->DSSetShader( 0, 0, 0 );
}

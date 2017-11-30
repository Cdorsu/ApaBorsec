#include "Terrain.h"



Terrain::Terrain( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	InitShaders( );
	InitHeightmap( L"Heightmaps/Heightmaps.bmp" );
	InitBuffers( );
	mTexture = new CTexture( );
	mTexture->Initialize( mDevice, L"Heightmaps/Heightmaps.bmp" );
}


Terrain::~Terrain( )
{
	for ( size_t i = 0; i < mBlobs.size( ); ++i )
	{
		if ( mBlobs[ i ] )
		{
			mBlobs[ i ]->Release( );
			mBlobs[ i ] = 0;
		}
	}
	if ( mVertexBuffer )
	{
		mVertexBuffer->Release( );
		mVertexBuffer = 0;
	}
	if ( mIndexBuffer )
	{
		mIndexBuffer->Release( );
		mIndexBuffer = 0;
	}
	if ( mVertexShader )
	{
		mVertexShader->Release( );
		mVertexShader = 0;
	}
	if ( mHullShader )
	{
		mHullShader->Release( );
		mHullShader = 0;
	}
	if ( mDomainShader )
	{
		mDomainShader->Release( );
		mDomainShader = 0;
	}
	if ( mPixelShader )
	{
		mPixelShader->Release( );
		mPixelShader = 0;
	}
	if ( mPerObjectBuffer )
	{
		mPerObjectBuffer->Release( );
		mPerObjectBuffer = 0;
	}
	if ( mPerFrameBuffer )
	{
		mPerFrameBuffer->Release( );
		mPerFrameBuffer = 0;
	}
	if ( mCellSpacingBuffer )
	{
		mCellSpacingBuffer->Release( );
		mCellSpacingBuffer = 0;
	}
	if ( mWrapSampler )
	{
		mWrapSampler->Release( );
		mWrapSampler = 0;
	}
}

void Terrain::InitShaders( )
{
	HRESULT hr;
	ID3DBlob *Errors;
	hr = D3DX11CompileFromFile( L"TerrainVertexShader.hlsl", nullptr, nullptr,
		"main", "vs_5_0", 0, 0, nullptr, &mBlobs[0], &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = mDevice->CreateVertexShader( mBlobs[ 0 ]->GetBufferPointer( ),
		mBlobs[ 0 ]->GetBufferSize( ),
		nullptr, &mVertexShader );
	if ( FAILED( hr ) )
		throw "Fraier";
	D3D11_INPUT_ELEMENT_DESC layout[ 2 ];
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

	int numElements = ARRAYSIZE( layout );

	hr = mDevice->CreateInputLayout( layout, numElements, mBlobs[ 0 ]->GetBufferPointer( ),
		mBlobs[ 0 ]->GetBufferSize( ), &mLayout );
	if ( FAILED( hr ) )
		throw "Failed";

	hr = D3DX11CompileFromFile( L"TerrainHullShader.hlsl", nullptr, nullptr,
		"main", "hs_5_0", 0, 0, nullptr, &mBlobs[ 1 ], &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = mDevice->CreateHullShader( mBlobs[ 1 ]->GetBufferPointer( ),
		mBlobs[ 1 ]->GetBufferSize( ),
		nullptr, &mHullShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	hr = D3DX11CompileFromFile( L"TerrainDomainShader.hlsl", nullptr, nullptr,
		"main", "ds_5_0", 0, 0, nullptr, &mBlobs[ 2 ], &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = mDevice->CreateDomainShader( mBlobs[ 2 ]->GetBufferPointer( ),
		mBlobs[ 2 ]->GetBufferSize( ),
		nullptr, &mDomainShader );
	if ( FAILED( hr ) )
		throw "Fraier";

	hr = D3DX11CompileFromFile( L"TerrainPixelShader.hlsl", nullptr, nullptr,
		"main", "ps_5_0", 0, 0, nullptr, &mBlobs[ 3 ], &Errors, nullptr );
	if ( FAILED( hr ) )
		throw "Fraier";
	hr = mDevice->CreatePixelShader( mBlobs[ 3 ]->GetBufferPointer( ),
		mBlobs[ 3 ]->GetBufferSize( ),
		nullptr, &mPixelShader );
	if ( FAILED( hr ) )
		throw "Fraier";
}

bool Terrain::InitHeightmap( LPWSTR lpPath )
{
	char mb[ MAX_PATH ];
	size_t converted = 0;
	wcstombs_s( &converted, mb, lpPath, MAX_PATH );
	FILE * Heightmap;
	int err;
	err = fopen_s( &Heightmap, mb, "rb" );
	if ( err )
		return false;

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread( &fileHeader, sizeof( BITMAPFILEHEADER ), 1, Heightmap );
	fread( &infoHeader, sizeof( BITMAPINFOHEADER ), 1, Heightmap );

	mNumRow = infoHeader.biWidth;
	mNumCol = infoHeader.biHeight;
	UCHAR * image = new UCHAR[ mNumRow * mNumCol * 3 ];

	fread( image, sizeof( UCHAR ), mNumRow * mNumCol * 3, Heightmap );

	mHeightmap.resize( mNumRow * mNumCol );
	float halfRow = ( float ) mNumRow / 2;
	float halfCol = ( float ) mNumCol / 2;
	int k = 0;
	for ( int i = 0; i < mNumRow; ++i )
	{
		for ( int j = 0; j < mNumCol; ++j )
		{
			int index = i * mNumRow + j;
			mHeightmap[ index ].Pos.x = j - halfCol;
			mHeightmap[ index ].Pos.z = i - halfRow;
			mHeightmap[ index ].Pos.y = image[ k ];
			k += 3;
		}
	}

	mCellTexSpacingU = 1.0f / mNumCol;
	mCellTexSpacingV = 1.0f / mNumRow;

	return true;
}

bool Terrain::InitBuffers( )
{
	HRESULT hr;
	mNumRowVertices = ( ( mNumRow - 1 ) / CellsPerPath ) + 1;
	mNumColVertices = ( ( mNumCol - 1 ) / CellsPerPath ) + 1;

	mNumPathVertices = mNumRowVertices * mNumColVertices;
	mNumQuadFaces = ( mNumRowVertices - 1 ) * ( mNumColVertices - 1 );

	ComputeBounds( );
	InitVertexBuffer( );
	InitIndexBuffer( );

	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, buffDesc );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SPerObject );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mPerObjectBuffer );
	if ( FAILED( hr ) )
		return false;

	ZeroMemory( &buffDesc, sizeof( D3D11_BUFFER_DESC ) );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SPerFrame );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mPerFrameBuffer );
	if ( FAILED( hr ) )
		return false;

	ZeroMemory( &buffDesc, sizeof( D3D11_BUFFER_DESC ) );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SCellInfo );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mCellSpacingBuffer );
	if ( FAILED( hr ) )
		return false;

	ZeroMemory( &buffDesc, sizeof( D3D11_BUFFER_DESC ) );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = sizeof( SLight );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	hr = mDevice->CreateBuffer( &buffDesc, nullptr, &mLightBuffer );
	if ( FAILED( hr ) )
		return false;

	ZeroMemoryAndDeclare( D3D11_SAMPLER_DESC, sampDesc );
	sampDesc.AddressU =
		sampDesc.AddressV =
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MipLODBias = 0;
	hr = mDevice->CreateSamplerState( &sampDesc, &mWrapSampler );

	return true;
}

void Terrain::ComputeBounds( )
{
	mBoundsY.resize( ( mNumColVertices - 1 ) * ( mNumRowVertices - 1 ) );
	for ( int i = 0; i < mNumRowVertices - 1; ++i )
	{
		for ( int j = 0; j < mNumColVertices - 1; ++j )
		{
			CalculateYBounds( i, j );
		}
	}
}

void Terrain::CalculateYBounds( int row, int col )
{
	int x0 = col * CellsPerPath;
	int x1 = ( col + 1 ) * CellsPerPath;

	int y0 = row * CellsPerPath;
	int y1 = ( row + 1 ) * CellsPerPath;

	float minY = +FLT_MAX;
	float maxY = -FLT_MAX;
	int height;

	for ( int y = y0; y <= y1; ++y )
	{
		for ( int x = x0; x <= x1; ++x )
		{
			height = y * mNumRow + x;
			minY = min( minY, mHeightmap[ height ].Pos.y );
			maxY = max( maxY, mHeightmap[ height ].Pos.y );
		}
	}
	mBoundsY[ row * ( mNumColVertices - 1 ) + col ] = DirectX::XMFLOAT2( minY, maxY );
}

void Terrain::InitVertexBuffer( )
{
	float halfWidth = GetWidth( ) / 2.0f;
	float halfDepth = GetHeight( ) / 2.0f;

	float patchWidth = ( float ) GetWidth( ) / ( mNumColVertices - 1 );
	float patchDepth = ( float ) GetHeight( ) / ( mNumRowVertices - 1 );

	mVertices.resize( mNumColVertices * mNumRowVertices );

	float z,x;
	float du, dv;

	du = 1.0f / ( mNumColVertices - 1 );
	dv = 1.0f / ( mNumRowVertices - 1 );

	for ( int i = 0; i < mNumRowVertices; ++i )
	{
		z = -halfDepth + i * patchDepth;
		for ( int j = 0; j < mNumColVertices; ++j )
		{
			x = -halfWidth + j * patchWidth;
			mVertices[ i * mNumColVertices + j ].Pos = DirectX::XMFLOAT3( x, 0.0f, z );
			
			mVertices[ i * mNumColVertices + j ].Tex.x = j * du;
			mVertices[ i * mNumColVertices + j ].Tex.y = i * dv;
		}
	}

	for ( int i = 0; i < mNumRowVertices - 1; ++i )
	{
		for ( int j = 0; j < mNumColVertices - 1; ++j )
		{
			int patchID;
			patchID = i * ( mNumColVertices - 1 ) + j;
			mVertices[ i * mNumColVertices + j ].Bounds = mBoundsY[ patchID ];
		}
	}

	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, buffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, buffData );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( SVertex ) * mVertices.size( );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	buffData.pSysMem = &mVertices[ 0 ];
	HRESULT hr;
	hr = mDevice->CreateBuffer( &buffDesc, &buffData, &mVertexBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";
}

void Terrain::InitIndexBuffer( )
{
	mIndices.resize( mNumQuadFaces * 4 );

	int k = 0;
	for ( int i = 0; i < mNumRowVertices - 1; ++i )
	{
		for ( int j = 0; j < mNumColVertices - 1; ++j )
		{
			mIndices[ k ] = i * mNumColVertices + j;
			mIndices[ k + 1 ] = i * mNumColVertices + j + 1;
			mIndices[ k + 2 ] = ( i + 1 ) * mNumColVertices + j;
			mIndices[ k + 3 ] = ( i + 1 ) * mNumColVertices + j + 1;

			k += 4;
		}
	}

	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, buffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, buffData );
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	buffDesc.ByteWidth = sizeof( DWORD ) * mIndices.size( );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	buffData.pSysMem = &mIndices[ 0 ];
	HRESULT hr;
	hr = mDevice->CreateBuffer( &buffDesc, &buffData, &mIndexBuffer );
	if ( FAILED( hr ) )
		throw "Fraier";
}

void Terrain::Update( float frameTime )
{
}

void Terrain::Render( DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection, DirectX::XMFLOAT3 const& EyePos, CLight* Light )
{
	static DirectX::XMMATRIX WVP;
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	static UINT Stride = sizeof( SVertex );
	static UINT Offset = 0;

	mContext->VSSetShader( mVertexShader, 0, 0 );
	mContext->HSSetShader( mHullShader, 0, 0 );
	mContext->DSSetShader( mDomainShader, 0, 0 );
	mContext->PSSetShader( mPixelShader, 0, 0 );

	ID3D11ShaderResourceView * SRV = mTexture->GetTexture( );
	mContext->VSSetShaderResources( 0, 1, &SRV );
	mContext->DSSetShaderResources( 0, 1, &SRV );
	mContext->PSSetShaderResources( 0, 1, &SRV );

	mContext->IASetVertexBuffers( 0, 1, &mVertexBuffer, &Stride, &Offset );
	mContext->IASetIndexBuffer( mIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetInputLayout( mLayout );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST );

	mContext->VSSetSamplers( 0, 1, &mWrapSampler );
	mContext->DSSetSamplers( 0, 1, &mWrapSampler );
	mContext->PSSetSamplers( 0, 1, &mWrapSampler );

	WVP = View * Projection;
	mContext->Map( mPerObjectBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( SPerObject* ) MappedSubresource.pData )->WVP = DirectX::XMMatrixTranspose( WVP );
	mContext->Unmap( mPerObjectBuffer, 0 );
	mContext->DSSetConstantBuffers( 0, 1, &mPerObjectBuffer );

	mContext->Map( mPerFrameBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( SPerFrame* ) MappedSubresource.pData )->CamPos = EyePos;
	mContext->Unmap( mPerFrameBuffer, 0 );
	mContext->HSSetConstantBuffers( 0, 1, &mPerFrameBuffer );

	mContext->Map( mCellSpacingBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( SCellInfo* ) MappedSubresource.pData )->CellSpacingU = mCellTexSpacingU;
	( ( SCellInfo* ) MappedSubresource.pData )->CellSpacingV = mCellTexSpacingV;
	mContext->Unmap( mCellSpacingBuffer, 0 );
	mContext->PSSetConstantBuffers( 0, 1, &mCellSpacingBuffer );

	mContext->Map( mLightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( SLight* ) MappedSubresource.pData )->Ambient = Light->GetAmbientColor( );
	( ( SLight* ) MappedSubresource.pData )->Diffuse = Light->GetDiffuseColor( );
	( ( SLight* ) MappedSubresource.pData )->LightDir = Light->GetDirection( );
	mContext->Unmap( mLightBuffer, 0 );
	mContext->PSSetConstantBuffers( 1, 1, &mLightBuffer );

	mContext->DrawIndexed( mNumQuadFaces * 4, 0, 0 );

	mContext->HSSetShader( 0, 0, 0 );
	mContext->DSSetShader( 0, 0, 0 );
}

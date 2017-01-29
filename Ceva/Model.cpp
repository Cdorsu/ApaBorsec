#include "Model.h"



CModel::CModel()
{
	ZeroMemory( this, sizeof( CModel ) );
}

bool CModel::Initialize( ID3D11Device * device )
{
	Texture = new CTexture();
	if (!Texture->Initialize( device, L"AnaS.dds" ))
		return false;

	HRESULT hr;
	Vertex Vertices[] =
	{
		// Front face
		Vertex( -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f ), // 0
		Vertex(  1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f ), // 1
		Vertex(  1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f ), // 2
		Vertex( -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f ), // 3
		// Back face
		Vertex( -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f ), // 4
		Vertex(  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f ), // 5
		Vertex(  1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f ), // 6
		Vertex( -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f ), // 7
		// Top face
		Vertex( -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f ), // 8
		Vertex(  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f ), // 9
		Vertex(  1.0f, 1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f ), // 10
		Vertex( -1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f ), // 11
		// Bottom face
		Vertex( -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f ), // 12
		Vertex(  1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f ), // 13
		Vertex(  1.0f, -1.0f,-1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f ), // 14
		Vertex( -1.0f, -1.0f,-1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f ), // 15
		// Left face
		Vertex( -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f ), // 16
		Vertex( -1.0f, 1.0f,-1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f ), // 17
		Vertex( -1.0f,-1.0f,-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f ), // 18
		Vertex( -1.0f,-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f ), // 19
		// Right face
		Vertex( 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f ), // 20
		Vertex( 1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f ), // 21
		Vertex( 1.0f,-1.0f,-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f ), // 22
		Vertex( 1.0f,-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f ), // 23
	};
	VertexCount = sizeof(Vertices)/sizeof(Vertices[0]);
	DWORD Indices[] =
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
	IndexCount = sizeof( Indices ) / sizeof( Indices[0] );
	World = DirectX::XMMatrixIdentity();
#pragma region Calculate vectors

	int size = sizeof( Indices ) / sizeof( Indices[0] );
	for (int i = 0; i < size; i += 3)
	{
		math::XMFLOAT3 Tangent, Binormal, Normal;
		math::CalculateTangentandBinormal( Vertices[Indices[i]].Position, Vertices[Indices[i + 1]].Position, Vertices[Indices[i + 2]].Position,
			Vertices[Indices[i]].Texture, Vertices[Indices[i + 1]].Texture, Vertices[Indices[i + 2]].Texture, Tangent, Binormal );
		Vertices[Indices[i]].Tangent = Tangent;
		Vertices[Indices[i + 1]].Tangent = Tangent;
		Vertices[Indices[i + 2]].Tangent = Tangent;
		Vertices[Indices[i]].Binormal = Binormal;
		Vertices[Indices[i + 1]].Binormal = Binormal;
		Vertices[Indices[i + 2]].Binormal = Binormal;
	}

#pragma endregion
	D3D11_BUFFER_DESC buffDesc = { 0 };
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( Vertices );
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffData.pSysMem = Vertices;
	hr = device->CreateBuffer( &buffDesc, &buffData, &VertexBuffer );
	if (FAILED( hr ))
		return false;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	buffDesc.ByteWidth = sizeof( Indices );
	buffData.pSysMem = Indices;
	hr = device->CreateBuffer( &buffDesc, &buffData, &IndexBuffer );
	if (FAILED( hr ))
		return false;

	return true;
}

bool CModel::Initialize( ID3D11Device * device, LPWSTR lpFilepath, LPWSTR lpTexture, LPWSTR Bumpmap, LPWSTR Specular )
{
	HRESULT hr;
	Texture = new CTexture();
	if (!Texture->Initialize( device, lpTexture ))
		return false;
	BumpMap = new CTexture();
	if (!BumpMap->Initialize( device, Bumpmap ))
		return false;
	SpecularMap = new CTexture();
	if (!SpecularMap->Initialize( device, Specular ))
		return false;

	std::wifstream ifs( lpFilepath );
	if (!ifs.is_open())
		return false;
	wchar_t check = NULL;
	while (check != ':')
	{
		check = ifs.get();
	}
	ifs >> VertexCount;
	IndexCount = VertexCount;
	std::vector<DWORD> indices( IndexCount );
	std::vector<Vertex> vertices( VertexCount );
	check = ifs.get();
	while (check != ':')
	{
		check = ifs.get();
	}
	check = ifs.get();
	for (UINT i = 0; i < VertexCount; ++i)
	{
		float x, y, z, u, v, nx, ny, nz;
		indices[i] = i;
		ifs >> x >> y >> z >> u >> v >> nx >> ny >> nz;
		vertices[i].Position = DirectX::XMFLOAT3( x, y, z );
		vertices[i].Texture = DirectX::XMFLOAT2( u, v );
		vertices[i].Normal = DirectX::XMFLOAT3( nx, ny, nz );
	}
	ifs.close();
#pragma region Calculate vectors

	for (UINT i = 0; i < indices.size() / 3; ++i)
	{
		math::XMFLOAT3 Tangent, Binormal, Normal;
		math::CalculateTangentandBinormal( vertices[indices[(i * 3)]].Position, vertices[indices[(i * 3) + 1]].Position, vertices[indices[(i * 3) + 2]].Position,
			vertices[indices[(i * 3)]].Texture, vertices[indices[(i * 3) + 1]].Texture, vertices[indices[(i * 3) + 2]].Texture, Tangent, Binormal );
		vertices[indices[(i * 3)]].Tangent = Tangent;
		vertices[indices[(i * 3) + 1]].Tangent = Tangent;
		vertices[indices[(i * 3) + 2]].Tangent = Tangent;
		vertices[indices[(i * 3)]].Binormal = Binormal;
		vertices[indices[(i * 3) + 1]].Binormal = Binormal;
		vertices[indices[(i * 3) + 2]].Binormal = Binormal;
		/*math::CalculateNormal( Tangent, Binormal, Normal );
		vertices[indices[(i * 3)]].Normal = Normal;
		vertices[indices[(i * 3) + 1]].Normal = Normal;
		vertices[indices[(i * 3) + 2]].Normal = Normal;*/
	}

#pragma endregion
	World = DirectX::XMMatrixIdentity();
	D3D11_BUFFER_DESC buffDesc = { 0 };
	D3D11_SUBRESOURCE_DATA buffData = { 0 };
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof( Vertex ) * VertexCount;
	buffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	buffData.pSysMem = &vertices[0];
	hr = device->CreateBuffer( &buffDesc, &buffData, &VertexBuffer );
	if (FAILED( hr ))
		return false;
	buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	buffDesc.ByteWidth = sizeof( DWORD ) * IndexCount;
	buffData.pSysMem = &indices[0];
	buffData.SysMemPitch = 0;
	buffData.SysMemSlicePitch = 0;
	hr = device->CreateBuffer( &buffDesc, &buffData, &IndexBuffer );
	if (FAILED( hr ))
		return false;
	return true;
}

void CModel::Render( ID3D11DeviceContext * context )
{
	UINT Stride = sizeof( Vertex );
	UINT Offset = 0;
	context->IASetVertexBuffers( 0, 1, &VertexBuffer, &Stride, &Offset );
	context->IASetIndexBuffer( IndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

void CModel::Shutdown()
{
	SpecularMap->Shutdown();
	delete SpecularMap;
	BumpMap->Shutdown();
	delete BumpMap;
	Texture->Shutdown();
	delete Texture;
	SafeRelease( VertexBuffer );
	SafeRelease( IndexBuffer );
}

CModel::~CModel()
{
	Shutdown();
}

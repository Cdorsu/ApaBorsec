#pragma once

#include "common.h"
#include "Texture.h"

#include <DirectXMath.h>
#include <fstream>
#include <Windows.h>

class FontClass
{
private:
	struct SChar
	{
		float left, right;
		int width;
	};
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
		SVertex()
		{
			ZeroMemory( this, sizeof( SVertex ) );
		};
		SVertex( float x, float y, float z,
			float u, float v )
			:Position( x, y, z ), Texture( u, v ) {};
	};
private:
	static SChar *Characters;
	static CTexture *Texture;
private:
	static bool LoadFontData( LPWSTR lpData );
public:
	FontClass();
	~FontClass();
public:
	static bool Initialize( ID3D11Device * device, LPWSTR lpTexture, LPWSTR lpData );
	static void Shutdown();
	static void BuildVertexArray( void* vertices, const char* Sentence, float DrawX, float DrawY );
public:
	static inline ID3D11ShaderResourceView* GetTexture() { return Texture->GetTexture(); };
};


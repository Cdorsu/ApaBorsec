#include "FontClass.h"

FontClass::SChar* FontClass::Characters = nullptr;
CTexture* FontClass::Texture = nullptr;


FontClass::FontClass()
{
	ZeroMemory( this, sizeof( FontClass ) );
}

bool FontClass::Initialize( ID3D11Device * device, LPWSTR lpTexture, LPWSTR lpData )
{
	Texture = new CTexture();
	if (!Texture->Initialize( device, lpTexture ))
		return false;
	if (!LoadFontData( lpData ))
		return false;

	return true;
}

bool FontClass::LoadFontData( LPWSTR lpData )
{
	wchar_t check;
	std::wifstream ifs( lpData );
	if (!ifs.is_open())
		return false;
	Characters = new SChar[95];
	for (int i = 0; i < 95; ++i)
	{
		ifs.get( check );
		while (check != ' ')
		{
			ifs.get( check );
		}
		ifs.get( check );
		while (check != ' ')
		{
			ifs.get( check );
		}
		ifs >> Characters[i].left;
		ifs >> Characters[i].right;
		ifs >> Characters[i].width;
	}
	
	ifs.close();
	return true;
}

void FontClass::BuildVertexArray( void* vertices, const char* Sentence, float DrawX, float DrawY )
{
	float StartX = DrawX;
	SVertex *verticesPtr = (SVertex*)(vertices);
	int length = strlen( Sentence );
	int caracter, index = 0;
	/*FontClass::SVertex Avertices[] =
	{
		FontClass::SVertex( -300.0f, 300.0f, 0.0f, 0.242188, 0.0f ), // 0
		FontClass::SVertex( 300.0f, 300.0f, 0.0f, 0.249023, 0.0f ), // 1
		FontClass::SVertex( 300.0f, -300.0f, 0.0f, 0.249023, 1.0f ), // 2
		FontClass::SVertex( -300.0f, -300.0f, 0.0f, 0.242188, 1.0f ), // 3
	};
	memcpy( vertices, Avertices, sizeof( Avertices ) );*/
	for (int i = 0; i < length; ++i)
	{
		caracter = ((int)Sentence[i] - 32);
		if (caracter <= 0.0f)
		{
			if (Sentence[i] == '\n')
			{
				DrawX = StartX;
				DrawY -= 17.0f;
			}
			else
				DrawX += 3.0f;
		}
		else
		{
			// First triangle
			verticesPtr[index].Position = DirectX::XMFLOAT3( DrawX, DrawY, 0.0f ); // Top-left vertex
			verticesPtr[index].Texture = DirectX::XMFLOAT2( Characters[caracter].left, 0.0f );
			index++;

			verticesPtr[index].Position = DirectX::XMFLOAT3( DrawX + Characters[caracter].width, DrawY, 0.0f ); // Top-right vertex
			verticesPtr[index].Texture = DirectX::XMFLOAT2( Characters[caracter].right, 0.0f );
			index++;

			verticesPtr[index].Position = DirectX::XMFLOAT3( DrawX + Characters[caracter].width, DrawY - 16, 0.0f ); // Bottom-right vertex
			verticesPtr[index].Texture = DirectX::XMFLOAT2( Characters[caracter].right, 1.0f );
			index++;

			// Second triangle
			verticesPtr[index].Position = DirectX::XMFLOAT3( DrawX, DrawY, 0.0f ); // Top-left vertex
			verticesPtr[index].Texture = DirectX::XMFLOAT2( Characters[caracter].left, 0.0f );
			index++;

			verticesPtr[index].Position = DirectX::XMFLOAT3( DrawX + Characters[caracter].width, DrawY - 16, 0.0f ); // Bottom-right vertex
			verticesPtr[index].Texture = DirectX::XMFLOAT2( Characters[caracter].right, 1.0f );
			index++;

			verticesPtr[index].Position = DirectX::XMFLOAT3( DrawX, DrawY - 16, 0.0f ); // Bottom-left vertex
			verticesPtr[index].Texture = DirectX::XMFLOAT2( Characters[caracter].left, 1.0f );
			index++;

			DrawX += Characters[caracter].width + 1.0f;
		}
	}
}

void FontClass::Shutdown()
{
	Texture->Shutdown();
	delete Texture;

	delete Characters;
}

FontClass::~FontClass()
{
}

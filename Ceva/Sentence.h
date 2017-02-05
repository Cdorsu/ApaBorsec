#pragma once

#include "common.h"
#include "FontClass.h"

#include <vector>
#include <d3d11.h>
#include <windows.h>

class CSentence
{
	typedef FontClass::SVertex SVertex;
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VertexCount;
	UINT IndexCount;
	std::string Sentence;
	UINT ScreenWidth, ScreenHeight;
	UINT maxLength;
	float CurrentX, CurrentY;
public:
	CSentence();
	~CSentence();
private:
	void RenderBuffers( ID3D11DeviceContext * context );
public:
	bool Initialize( ID3D11Device * device, char * sentence, int ScreenWidth, int ScreenHeigth, float x, float y );
	bool Update( ID3D11DeviceContext * context, char* sentence, size_t length, float x = -1.0f, float y = -1.0f );
	void Render( ID3D11DeviceContext * context);
	void Shutdown();
public:
	inline UINT GetVertexCount() { return VertexCount; };
	inline UINT GetIndexCount() { return IndexCount; };
	inline const char* GetSentence() { return Sentence.c_str(); };
};


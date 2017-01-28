#pragma once

#include "common.h"
#include "FontClass.h"

#include <vector>
#include <d3d11.h>
#include <windows.h>

// Propozitie smechera
class CSentence
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VertexCount;
	UINT IndexCount;
	std::string Sentence;
	UINT PreviousX, PreviousY;
	UINT ScreenWidth, ScreenHeight;
public:
	CSentence();
	~CSentence();
private:
	void RenderBuffers( ID3D11DeviceContext * context );
public:
	bool Initialize( ID3D11Device * device, char * sentence, int ScreenWidth, int ScreenHeigth );
	bool Update( ID3D11Device * device, char* sentence );
	void Render( ID3D11DeviceContext * context, float x, float y );
	void Shutdown();
public:
	inline UINT GetVertexCount() { return VertexCount; };
	inline UINT GetIndexCount() { return IndexCount; };
	inline const char* GetSentence() { return Sentence.c_str(); };
};


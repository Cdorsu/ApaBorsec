#pragma once

#include "common.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>

class CTexture
{
	ID3D11ShaderResourceView *Texture;
public:
	CTexture();
	~CTexture();
public:
	bool Initialize( ID3D11Device * device, LPWSTR filepath );
	void Shutdown();
public:
	inline ID3D11ShaderResourceView* GetTexture() { return Texture; };

};

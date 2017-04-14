#pragma once

#include "Texture.h"
#include "RenderTexture.h"
#include "VerticalBlurShader.h"
#include "HorizontalBlurShader.h"
#include "C2DShader.h"
#include "MaskShader.h"
#include "CombineTextureShader.h"
#include "BitmapClass.h"

#include <d3d11.h>
#include <D3DX11.h>

class CGlow
{
	CTexture *m_Texture;
	CTexture *m_Glowmap;
	CHorizontalBlurShader *m_HorizontalBlurShader;
	CVerticalBlurShader *m_VerticalBlurShader;
	C2DShader *m_2DShader;
	CMaskShader *m_MaskShader;
	CCombineTextureShader *m_CombineTextureShader;
	CRenderTexture *m_ToGlow;
	CRenderTexture *m_DownSampleTexture;
	CRenderTexture *m_HorizontalBlurTexture;
	CRenderTexture *m_VerticalBlurTexture;
	CRenderTexture *m_CombinedTextures;
	BitmapClass *m_Button;
	BitmapClass *m_DownSampleButton;
	UINT m_WindowWidth;
	UINT m_WindowHeight;
	UINT m_ButtonWidth;
	UINT m_ButtonHeight;
public:
	CGlow( );
	~CGlow( );
public:
	bool Initialize( ID3D11Device * device, LPWSTR lpTexture, LPWSTR lpGlowmap,
		CHorizontalBlurShader * HorizontalBlurShader, CVerticalBlurShader * VerticalBlurShader,
		C2DShader * twShader, CMaskShader * MaskShader, CCombineTextureShader * CombineTexture,
		UINT WindowWidth, UINT WindowHeight,
		UINT ButtonWidth, UINT ButtonHeight, float FOV, float camNear, float camFar );
	void Render( ID3D11DeviceContext * context );
	void Shutdown( );
public:
	ID3D11ShaderResourceView* GetTexture( );
};


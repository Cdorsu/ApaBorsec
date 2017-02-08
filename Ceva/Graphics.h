#pragma once
#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include "D3DClass.h"
#include "Model.h"
#include "SimpleShader.h"
#include "ReflectionShader.h"
#include "WaterShader.h"
#include "Camera.h"
#include "Light.h"
#include "C2DShader.h"
#include "FogShader.h"
#include "TextureShader.h"
#include "BitmapClass.h"
#include "Sentence.h"
#include "RenderTexture.h"


class CGraphics sealed
{
	D3DClass *m_d3d;
	BitmapClass *m_Cursor;
	CSimpleShader *m_NoPlaneClippingShader;
	CSimpleShader *m_PlaneClippingShader;
	CSimpleShader *m_FaddingShader;
	C2DShader *m_2DShader;
	CFogShader *m_LinearFogShader;
	CFogShader *m_ExponentialFogShader;
	CFogShader *m_ExponentialFogShader2;
	CReflectionShader *m_ReflectionShader;
	CTextureShader *m_TextureShader;
	CWaterShader *m_WaterShader;
	CCamera *m_Camera;
	CModel *m_Bath;
	CModel *m_Wall;
	CModel *m_Ground;
	CModel *m_Water;
	CLight *Light;
	CSentence *m_FPSMessage;
	CSentence *m_FrameTimeMessage;
	CSentence *m_Cheat;
	CSentence *m_CursorX;
	CSentence *m_CursorY;
	BitmapClass *m_RenderTexture;
	CRenderTexture *m_RefractionTexture;
	CRenderTexture *m_ReflectionTexture;
	DirectX::XMFLOAT4 m_ClippingPlane;
	UINT m_WindowWidth;
	UINT m_WindowHeight;
	UINT m_RenderCount;
	float m_fStartLoadingTime;
	float m_fLoadingTime;
	float m_fFadeAmount;
	bool m_bIncrease;
	float m_fWaterHeight;
	float m_fWaterTranslation;
public:
	CGraphics();
	~CGraphics();
public:
	bool Initialize( HINSTANCE hInstance, HWND hWnd, UINT WindowWidth, UINT WindowHeight );
	void Frame( bool RenderMenu, DWORD dwFramesPerSecond = 0, float fFrameTime = 0.0f, UINT MouseX = 0, UINT MouseY = 0, char * cheat = "" );
	void Shutdown();
private:
	void Render( bool RenderMenu, char * Cheat, UINT MouseX, UINT MouseY );
	void Update( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * cheat );
};


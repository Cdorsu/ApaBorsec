#pragma once
#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include "D3DClass.h"
#include "Model.h"
#include "SimpleShader.h"
#include "Camera.h"
#include "Light.h"
#include "C2DShader.h"
#include "FogShader.h"
#include "BitmapClass.h"
#include "Sentence.h"
#include "RenderTexture.h"


class CGraphics sealed
{
	D3DClass *m_d3d;
	CModel *m_Triangle;
	CModel *m_Model;
	CModel *m_Floor;
	BitmapClass *m_Cursor;
	CSimpleShader *m_NoPlaneClippingShader;
	CSimpleShader *m_PlaneClippingShader;
	C2DShader *m_2DShader;
	CFogShader *m_LinearFogShader;
	CFogShader *m_ExponentialFogShader;
	CFogShader *m_ExponentialFogShader2;
	CCamera *m_Camera;
	CCamera *m_Up;
	CLight *Light;
	CSentence *m_FPSMessage;
	CSentence *m_FrameTimeMessage;
	CSentence *m_Cheat;
	CSentence *m_CursorX;
	CSentence *m_CursorY;
	BitmapClass *m_RenderTexture;
	CRenderTexture *m_TextureRenderer;
	DirectX::XMFLOAT4 m_ClippingPlane;
	UINT m_WindowWidth;
	UINT m_WindowHeight;
	UINT m_RenderCount;
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


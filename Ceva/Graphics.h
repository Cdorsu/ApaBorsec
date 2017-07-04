#pragma once
#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include "D3DClass.h"
#include "Model.h"
#include "SimpleShader.h"
#include "ReflectionShader.h"
#include "WaterShader.h"
#include "GlassShader.h"
#include "DepthShader.h"
#include "Camera.h"
#include "Light.h"
#include "PointLight.h"
#include "C2DShader.h"
#include "FogShader.h"
#include "TextureShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "InstanceShader.h"
#include "BitmapClass.h"
#include "Sentence.h"
#include "RenderTexture.h"
#include "ShadowShader.h"
#include "MultipleShadowShader.h"
#include "SoftShadowShader.h"
#include "ProjectionShader.h"
#include "ProjectiveLightShader.h"
#include "MaskShader.h"
#include "Glow.h"
#include "CombineTextureShader.h"


class CGraphics sealed
{
	static constexpr float camNear = 0.1f;
	static constexpr float camFar = 1000.0f;
	static constexpr float FOV = 0.5f * FLOAT_PI;
	static constexpr float LightFOV = 0.3f * FLOAT_PI;
	static constexpr float SHADOW_WIDTH = 1024;
	static constexpr float SHADOW_HEIGHT = 1024;
	static constexpr UINT ButtonWidth = 150;
	static constexpr UINT ButtonHeight = 50;
private:
	D3DClass *m_d3d;
	BitmapClass *m_Cursor;
	BitmapClass *m_DebugWindow;
	CRenderTexture *m_DebugWindowTexture;
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
	CGlassShader *m_GlassShader;
	CDepthShader *m_DepthShader;
	CShadowShader *m_ColorShadowShader;
	CShadowShader *m_BWShadowShader;
	CMultipleShadowShader *m_MultipleShadowShader;
	CSoftShadowShader *m_SoftShadowShader;
	CHorizontalBlurShader *m_HorizontalBlur;
	CVerticalBlurShader *m_VerticalBlur;
	CInstanceShader *m_InstanceShader;
	CProjectionShader *m_ProjectionShader;
	CProjectiveLightShader *m_ProjectiveLightShader;
	CMaskShader *m_MaskShader;
	CCombineTextureShader *m_CombineTextureShader;
	CCamera *m_Camera;
	CLight *Light;
	CPointLight *PointLight;
	CSentence *m_FPSMessage;
	CSentence *m_FrameTimeMessage;
	CSentence *m_Cheat;
	BitmapClass *m_Button;
	CGlow *m_GlowTest;

	CModel *m_Sphere;
	CModel *m_Floor;
	CModel *m_Wall;
	CModel *m_Mirror;

	UINT m_WindowWidth;
	UINT m_WindowHeight;
	
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


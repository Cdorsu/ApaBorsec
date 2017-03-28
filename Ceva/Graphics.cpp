#include "Game.h"



CGraphics::CGraphics()
{
	ZeroMemory( this, sizeof( CGraphics ) );
}

bool CGraphics::Initialize( HINSTANCE hInstance, HWND hWnd, UINT WindowWidth, UINT WindowHeight )
{
	m_WindowWidth = WindowWidth;
	m_WindowHeight = WindowHeight;
	m_d3d = new D3DClass();
	if (!m_d3d->Initialize( hInstance, hWnd, WindowWidth, WindowHeight, camNear, camFar ))
		return false;
	m_Cursor = new BitmapClass();
	if (!m_Cursor->Initialize( m_d3d->GetDevice(), L"data\\Cursor.dds", WindowWidth, WindowHeight, 32, 32 ))
		return false;
	m_DebugWindow = new BitmapClass( );
	if ( !m_DebugWindow->Initialize( m_d3d->GetDevice( ), L"", WindowWidth, WindowHeight, 400, 400 ) )
		return false;
	m_DebugWindowTexture = new CRenderTexture( );
	if ( !m_DebugWindowTexture->Initialize( m_d3d->GetDevice( ), WindowWidth, WindowHeight, FOV, camNear, camFar ) )
		return false;
	m_NoPlaneClippingShader = new CSimpleShader();
	if (!m_NoPlaneClippingShader->Initialize( m_d3d->GetDevice() ))
		return false;
	m_PlaneClippingShader = new CSimpleShader( );
	if ( !m_PlaneClippingShader->Initialize( m_d3d->GetDevice( ), CSimpleShader::EType::PlaneClipping ) )
		return false;
	m_FaddingShader = new CSimpleShader( );
	if ( !m_FaddingShader->Initialize( m_d3d->GetDevice( ), CSimpleShader::EType::NoPlaneClipping, true ) )
		return false;
	m_TextureShader = new CTextureShader( );
	if ( !m_TextureShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_LinearFogShader = new CFogShader();
	if (!m_LinearFogShader->Initialize(m_d3d->GetDevice(), CFogShader::EFogType::LinearFog))
		return false;
	m_ExponentialFogShader = new CFogShader();
	if (!m_ExponentialFogShader->Initialize(m_d3d->GetDevice(), CFogShader::EFogType::ExponentialFog))
		return false;
	m_ExponentialFogShader2 = new CFogShader();
	if (!m_ExponentialFogShader2->Initialize(m_d3d->GetDevice(), CFogShader::EFogType::ExponentialFog2))
		return false;
	m_ReflectionShader = new CReflectionShader( );
	if ( !m_ReflectionShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_GlassShader = new CGlassShader( );
	if ( !m_GlassShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_WaterShader = new CWaterShader( );
	if ( !m_WaterShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_2DShader = new C2DShader();
	if (!m_2DShader->Initialize( m_d3d->GetDevice() ))
		return false;
	m_DepthShader = new CDepthShader( );
	if ( !m_DepthShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_ShadowShader = new CShadowShader( );
	if ( !m_ShadowShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_HorizontalBlur = new CHorizontalBlurShader( );
	if ( !m_HorizontalBlur->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_VerticalBlur = new CVerticalBlurShader( );
	if ( !m_VerticalBlur->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_InstanceShader = new CInstanceShader( );
	if ( !m_InstanceShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_Camera = new CCamera();
	if (!m_Camera->Initialize( DirectX::XMVectorSet( 0.0f, 0.0f, -10.0f, 1.0f ),
		FOV, (FLOAT)WindowWidth / WindowHeight, camNear, camFar, 15.0f ))
		return false;
	if (!FontClass::Initialize( m_d3d->GetDevice(), L"data\\font.dds", L"data\\fontdata.txt" ))
		return false;
	m_Cheat = new CSentence();
	if ( !m_Cheat->Initialize( m_d3d->GetDevice( ), "Here is a veeeery long string. Here will be wrote possible cheats",
		WindowWidth, WindowHeight, 1.0f, ( float ) WindowHeight - 17.0f ) )
		return false;
	m_FPSMessage = new CSentence();
	if ( !m_FPSMessage->Initialize( m_d3d->GetDevice( ), "Frames per second: 000", WindowWidth, WindowHeight, 1.0f, 69.0f ) )
		return false;
	m_FrameTimeMessage = new CSentence();
	if ( !m_FrameTimeMessage->Initialize( m_d3d->GetDevice( ), "Frame time: 0.00", WindowWidth, WindowHeight, 1.0f, 20.0f ) )
		return false;

	// Models
	m_Ground = new CModel( );
	if ( !m_Ground->Initialize( m_d3d->GetDevice( ), L"data\\ground.txt", L"data\\ground01.dds" ) )
		return false;
	m_Cube = new CModel( );
	if ( !m_Cube->Initialize( m_d3d->GetDevice( ), L"data\\cube.txt", L"data\\marble01.dds" ) )
		return false;
	m_Sphere = new CModel( );
	if ( !m_Sphere->Initialize( m_d3d->GetDevice( ), L"data\\sphere.txt", L"data\\wall01.dds" ) )
		return false;

	m_Depthmap = new CRenderTexture( );
	if ( !m_Depthmap->Initialize( m_d3d->GetDevice( ), ( UINT ) SHADOW_WIDTH, ( UINT ) SHADOW_HEIGHT, LightFOV, camNear, camFar ) )
		return false;

	Light = new CLight();
	Light->SetSpecularColor( common::HexToRGB( 0xFFFFFF ) );
	Light->SetAmbientColor( common::Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
	Light->SetDiffuseColor( common::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
	Light->SetDirection( DirectX::XMFLOAT3( 0.5f, -1.0f, 1.0f ) );
	Light->SetSpecularPower( 32.0f );

	PointLight = new CPointLight( );
	PointLight->SetPosition( DirectX::XMFLOAT3( 0.0f, 3.0f, -10.0f ) );
	PointLight->SetDiffuseColor( common::HexToRGB( 0x0000FF ) );
	PointLight->SetAttenuation( 0.4f, 0.2f, 0.0f );
	PointLight->SetRange( 0.0f ); // disable point light

	m_LightView = new CLightView( );
	if ( !m_LightView->Initialize( ) )
		return false;
	m_LightView->SetAmbient( common::Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
	m_LightView->SetDiffuse( common::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_LightView->SetFocus( DirectX::XMFLOAT3( -2.0f, 0.0f, 0.0f ) );
	m_LightView->SetPerspective( LightFOV, camNear, camFar, ( FLOAT ) SHADOW_WIDTH, ( FLOAT ) SHADOW_HEIGHT );
	m_LightView->GenerateProjectionMatrix( );
	

	return true;
}

void CGraphics::Update( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * cheat )
{
	static float LightX = 0.0f;
	static int delta = 1;

	if ( LightX > 7.0f || LightX < -7.0f )
		delta *= -1;
	LightX += delta * fFrameTime * 1.0f;
	m_LightView->SetPosition( DirectX::XMFLOAT3( LightX, 3.0f, 0.0f ) );

	m_Ground->Identity( );

	m_Cube->Identity( );
	m_Cube->Translate( 0.0f, 1.0f, 0.0f );

	m_Sphere->Identity( );
	m_Sphere->Translate( -3.0f, 1.0f, 0.0f );

	

	PointLight->SetPosition( m_Camera->GetCameraPosition( ) );
	static char buffer[ 500 ] = { 0 };
	sprintf_s( buffer, "Frames per second: %d", dwFramesPerSecond );
	m_FPSMessage->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), 1.0f, 1.0f );
	sprintf_s( buffer, "Frame time: %.2lf", fFrameTime );
	m_FrameTimeMessage->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), 1.0f, 18.0f );
	if (RenderMenu)
	{
		if ( strlen( cheat ) > 0 )
		{
			sprintf_s( buffer, ":%s:", cheat );
			m_Cheat->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), 1.0f, ( float ) m_WindowHeight - 17.0f );
		}
	}
	else
	{
		m_Camera->Update( fFrameTime );
		
	}
}

void CGraphics::Frame( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * Cheat )
{

	Update( RenderMenu, dwFramesPerSecond, fFrameTime, MouseX, MouseY, Cheat );
	Render( RenderMenu, Cheat, MouseX, MouseY );

}

void CGraphics::Render( bool RenderMenu, char * Cheat, UINT MouseX, UINT MouseY )
{
	m_Camera->Render();
	m_LightView->GenerateViewMatrix( );
	m_d3d->DisableCulling( );

	m_Depthmap->SetRenderTarget( m_d3d->GetImmediateContext( ) );
	m_Depthmap->BeginScene( m_d3d->GetImmediateContext( ), common::HexToRGB( 0x0 ) );

	m_Ground->Render( m_d3d->GetImmediateContext( ) );
	m_DepthShader->Render( m_d3d->GetImmediateContext( ), m_Ground->GetIndexCount( ),
		m_Ground->GetWorld( ), m_LightView->GetView( ), m_LightView->GetProjection( ));

	m_Sphere->Render( m_d3d->GetImmediateContext( ) );
	m_DepthShader->Render( m_d3d->GetImmediateContext( ), m_Sphere->GetIndexCount( ),
		m_Sphere->GetWorld( ), m_LightView->GetView( ), m_LightView->GetProjection( ) );

	m_Cube->Render( m_d3d->GetImmediateContext( ) );
	m_DepthShader->Render( m_d3d->GetImmediateContext( ), m_Cube->GetIndexCount( ),
		m_Cube->GetWorld( ), m_LightView->GetView( ), m_LightView->GetProjection( ) );

	m_Ground->Render( m_d3d->GetImmediateContext( ) );

	m_d3d->ResetViewPort( );
	m_d3d->EnableBackBuffer( );
	m_d3d->BeginScene( );

	m_Ground->Render( m_d3d->GetImmediateContext( ) );
	m_ShadowShader->Render( m_d3d->GetImmediateContext( ), m_Ground->GetIndexCount( ),
		m_Ground->GetTexture( ), m_Depthmap->GetTexture( ), m_Ground->GetWorld( ),
		m_Camera->GetView( ), m_Camera->GetProjection( ), m_LightView );

	m_Sphere->Render( m_d3d->GetImmediateContext( ) );
	m_ShadowShader->Render( m_d3d->GetImmediateContext( ), m_Sphere->GetIndexCount( ),
		m_Sphere->GetTexture( ), m_Depthmap->GetTexture( ), m_Sphere->GetWorld( ),
		m_Camera->GetView( ), m_Camera->GetProjection( ), m_LightView );

	m_Cube->Render( m_d3d->GetImmediateContext( ) );
	m_ShadowShader->Render( m_d3d->GetImmediateContext( ), m_Cube->GetIndexCount( ),
		m_Cube->GetTexture( ), m_Depthmap->GetTexture( ), m_Cube->GetWorld( ),
		m_Camera->GetView( ), m_Camera->GetProjection( ), m_LightView );

#pragma region Draw UI
	m_d3d->EnableAlphaBlending( );

	if ( RenderMenu )
	{
		if ( strlen( Cheat ) > 0 )
		{
			m_Cheat->Render( m_d3d->GetImmediateContext( ) );
			m_2DShader->Render( m_d3d->GetImmediateContext( ), m_Cheat->GetIndexCount( ), FontClass::GetTexture( ),
				DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_d3d->GetOrthoMatrix( ), common::HexToRGB( 0xFFFF00 ) );
		}

		m_Cursor->Render( m_d3d->GetImmediateContext( ), MouseX, MouseY );
		m_2DShader->Render( m_d3d->GetImmediateContext( ), m_Cursor->GetIndexCount( ), m_Cursor->GetTexture( ),
			DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_d3d->GetOrthoMatrix( ) );
	}
	m_d3d->DisableAlphaBlending( );
#pragma endregion

	m_d3d->EndScene();
}

void CGraphics::Shutdown()
{
	FontClass::Shutdown();

	m_Cheat->Shutdown();
	delete m_Cheat;

	m_FrameTimeMessage->Shutdown();
	delete m_FrameTimeMessage;

	m_FPSMessage->Shutdown();
	delete m_FPSMessage;

	m_2DShader->Shutdown();
	delete m_2DShader;

	m_NoPlaneClippingShader->Shutdown();
	delete m_NoPlaneClippingShader;

	m_PlaneClippingShader->Shutdown( );
	delete m_PlaneClippingShader;

	m_FaddingShader->Shutdown( );
	delete m_FaddingShader;

	m_ReflectionShader->Shutdown( );
	delete m_ReflectionShader;

	m_WaterShader->Shutdown( );
	delete m_WaterShader;

	m_DepthShader->Shutdown( );
	delete m_DepthShader;

	m_ShadowShader->Shutdown( );
	delete m_ShadowShader;

	m_HorizontalBlur->Shutdown( );
	delete m_HorizontalBlur;

	m_VerticalBlur->Shutdown( );
	delete m_VerticalBlur;

	m_GlassShader->Shutdown( );
	delete m_GlassShader;

	m_TextureShader->Shutdown( );
	delete m_TextureShader;

	m_LinearFogShader->Shutdown();
	delete m_LinearFogShader;

	m_ExponentialFogShader->Shutdown();
	delete m_ExponentialFogShader;

	m_ExponentialFogShader2->Shutdown();
	delete m_ExponentialFogShader2;

	delete Light;

	m_Camera->Shutdown();
	delete m_Camera;
	
	m_Cursor->Shutdown();
	delete m_Cursor;

	m_DebugWindow->Shutdown( );
	delete m_DebugWindow;

	m_DebugWindowTexture->Shutdown( );
	delete m_DebugWindowTexture;

	m_InstanceShader->Shutdown( );
	delete m_InstanceShader;

	m_Ground->Shutdown( );
	delete m_Ground;

	m_Sphere->Shutdown( );
	delete m_Sphere;

	m_Cube->Shutdown( );
	delete m_Cursor;

	m_Depthmap->Shutdown( );
	delete m_Depthmap;

	m_d3d->Shutdown();
	delete m_d3d;
}

CGraphics::~CGraphics()
{
	Shutdown();
}
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
	m_Bath = new CModel( );
	if ( !m_Bath->Initialize( m_d3d->GetDevice( ), L"data\\bath.txt", L"data\\marble01.dds" ) )
		return false;
	m_Wall = new CModel( );
	if ( !m_Wall->Initialize( m_d3d->GetDevice( ), L"data\\wall.txt", L"data\\wall01.dds" ) )
		return false;
	m_Water = new CModel( );
	if ( !m_Water->Initialize( m_d3d->GetDevice( ), L"data\\water.txt", L"data\\water01.dds" ) )
		return false;
	m_Ground = new CModel( );
	if ( !m_Ground->Initialize( m_d3d->GetDevice( ), L"data\\ground.txt", L"data\\ground01.dds" ) )
		return false;
	
	// Textures
	ReflectionTexture = new CRenderTexture( );
	if ( !ReflectionTexture->Initialize( m_d3d->GetDevice( ), WindowWidth, WindowHeight, FOV, camNear, camFar ) )
		return false;
	RefractionTexture = new CRenderTexture( );
	if ( !RefractionTexture->Initialize( m_d3d->GetDevice( ), WindowWidth, WindowHeight, FOV, camNear, camFar ) )
		return false;

	Light = new CLight();
	Light->SetSpecularColor( common::HexToRGB( 0xFFFFFF ) );
	Light->SetAmbientColor( common::Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
	Light->SetDiffuseColor( common::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
	Light->SetDirection( DirectX::XMFLOAT3( 0.0f, -1.0f, 1.0f ) );
	Light->SetSpecularPower( 32.0f );

	PointLight = new CPointLight( );
	PointLight->SetPosition( DirectX::XMFLOAT3( 0.0f, 3.0f, -10.0f ) );
	PointLight->SetDiffuseColor( common::HexToRGB( 0x0000FF ) );
	PointLight->SetAttenuation( 0.0f, 0.2f, 0.0f );
	PointLight->SetRange( 0.0f ); // disable point light

	WaterTranslation = 0.f;
	WaterHeight = 1.75f;

	return true;
}

void CGraphics::Update( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * cheat )
{
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
	m_Wall->Identity( );
	m_Wall->Translate( 0.0f, 4.0f, 7.0f );

	m_Water->Identity( );
	m_Water->Translate( 0.0f, WaterHeight, 0.0f );

	m_Bath->Identity( );
	m_Bath->Translate( 0.0f, 1.0f, 0.0f );

	m_Ground->Identity( );

	WaterTranslation += 0.125f * fFrameTime;

	Update( RenderMenu, dwFramesPerSecond, fFrameTime, MouseX, MouseY, Cheat );
	Render( RenderMenu, Cheat, MouseX, MouseY );
}

void CGraphics::Render( bool RenderMenu, char * Cheat, UINT MouseX, UINT MouseY )
{
	m_Camera->Render();
	m_Camera->RenderReflection( WaterHeight );
	m_d3d->DisableCulling( );

	ReflectionTexture->SetRenderTarget( m_d3d->GetImmediateContext( ), m_d3d->GetDepthStencilView( ) );
	ReflectionTexture->BeginScene( m_d3d->GetImmediateContext( ), m_d3d->GetDepthStencilView( ), common::HexToRGB( 0x0 ) );

	m_Wall->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Wall->GetIndexCount( ), m_Wall->GetTexture( ),
		m_Wall->GetWorld( ), m_Camera->GetReflectView( ), m_Camera->GetProjection( ), Light, PointLight );

	RefractionTexture->SetRenderTarget( m_d3d->GetImmediateContext( ), m_d3d->GetDepthStencilView( ) );
	RefractionTexture->BeginScene( m_d3d->GetImmediateContext( ), m_d3d->GetDepthStencilView( ), common::HexToRGB( 0x0 ) );

	m_Bath->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Bath->GetIndexCount( ), m_Bath->GetTexture( ),
		m_Bath->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight,
		DirectX::XMFLOAT4( 0.0f, -1.0f, 0.0, WaterHeight ) );

	m_d3d->EnableBackBuffer( );
	m_d3d->BeginScene( );
	m_d3d->ResetViewPort( );

	m_Wall->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Wall->GetIndexCount( ), m_Wall->GetTexture( ), 
		m_Wall->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

	m_Bath->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Bath->GetIndexCount( ), m_Bath->GetTexture( ),
		m_Bath->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

	m_Water->Render( m_d3d->GetImmediateContext( ) );
	m_WaterShader->Render( m_d3d->GetImmediateContext( ), m_Water->GetIndexCount( ), ReflectionTexture->GetTexture( ),
		RefractionTexture->GetTexture( ), m_Water->GetTexture( ), m_Camera->GetReflectView( ), m_Water->GetWorld( ),
		m_Camera->GetView( ), m_Camera->GetProjection( ), WaterTranslation, 0.01f );

	m_Ground->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Ground->GetIndexCount( ), m_Ground->GetTexture( ),
		m_Ground->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

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

	m_InstanceShader->Shutdown( );
	delete m_InstanceShader;

	m_Wall->Shutdown( );
	delete m_Wall;

	m_Bath->Shutdown( );
	delete m_Bath;

	m_Water->Shutdown( );
	delete m_Water;

	m_Ground->Shutdown( );
	delete m_Ground;

	ReflectionTexture->Shutdown( );
	delete ReflectionTexture;

	RefractionTexture->Shutdown( );
	delete RefractionTexture;

	m_d3d->Shutdown();
	delete m_d3d;
}

CGraphics::~CGraphics()
{
	Shutdown();
}
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
	m_Floor = new CModel( );
	if ( !m_Floor->Initialize( m_d3d->GetDevice( ), L"data\\cube.txt", L"data\\seafloor.dds" ) )
		return false;
	m_RenderTexture = new CRenderTexture( );
	if ( !m_RenderTexture->Initialize( m_d3d->GetDevice( ), WindowWidth, WindowHeight, FOV, camNear, camFar ) )
		return false;
	UINT downSampleWidth = WindowWidth / 4;
	UINT downSampleHeight = WindowHeight / 4;
	m_DownSampleTexture = new CRenderTexture( );
	if ( !m_DownSampleTexture->Initialize( m_d3d->GetDevice( ), downSampleWidth, downSampleHeight, FOV, camNear, camFar ) )
		return false;
	m_HorizontalBlurTexture = new CRenderTexture( );
	if ( !m_HorizontalBlurTexture->Initialize( m_d3d->GetDevice( ), downSampleWidth, downSampleHeight, FOV, camNear, camFar ) )
		return false;
	m_VerticalBlurTexture = new CRenderTexture( );
	if ( !m_VerticalBlurTexture->Initialize( m_d3d->GetDevice( ), downSampleWidth, downSampleHeight, FOV, camNear, camFar ) )
		return false;
	m_UpSampleTexture = new CRenderTexture( );
	if ( !m_UpSampleTexture->Initialize( m_d3d->GetDevice( ), WindowWidth, WindowHeight, FOV, camNear, camFar ) )
		return false;
	m_DownSampleWindow = new BitmapClass( );
	if ( !m_DownSampleWindow->Initialize( m_d3d->GetDevice( ), L"",
		downSampleWidth, downSampleHeight, downSampleWidth, downSampleHeight ) )
		return false;
	m_UpSampleWindow = new BitmapClass( );
	if ( !m_UpSampleWindow->Initialize( m_d3d->GetDevice( ), L"",
		WindowWidth, WindowHeight, WindowWidth, WindowHeight ) )
		return false;


	Light = new CLight();
	Light->SetSpecularColor( common::HexToRGB( 0xFFFFFF ) );
	Light->SetAmbientColor( common::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
	Light->SetDiffuseColor( common::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
	Light->SetDirection( DirectX::XMFLOAT3( 0.0f, -1.0f, 0.5f ) );
	Light->SetSpecularPower( 32.0f );

	PointLight = new CPointLight( );
	PointLight->SetPosition( DirectX::XMFLOAT3( 0.0f, 3.0f, -10.0f ) );
	PointLight->SetDiffuseColor( common::HexToRGB( 0x0000FF ) );
	PointLight->SetAttenuation( 0.0f, 0.2f, 0.0f );
	PointLight->SetRange( 0.0f ); // disable point light

	return true;
}

void CGraphics::Update( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * cheat )
{
	PointLight->SetPosition( m_Camera->GetCameraPosition( ) );
	//m_Floor->Identity( );
	m_Floor->RotateY( 0.5f * fFrameTime );
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
	m_d3d->DisableCulling( );

	m_RenderTexture->SetRenderTarget( m_d3d->GetImmediateContext( ) );
	m_RenderTexture->BeginScene( m_d3d->GetImmediateContext( ), common::HexToRGB( 0x0 ) );

	m_Floor->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Floor->GetIndexCount( ), m_Floor->GetTexture( ),
		m_Floor->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

	m_DownSampleTexture->SetRenderTarget( m_d3d->GetImmediateContext( ) );
	m_DownSampleTexture->BeginScene( m_d3d->GetImmediateContext( ), common::HexToRGB( 0x00FF00 ) );

	m_DownSampleWindow->Render( m_d3d->GetImmediateContext( ), 0, 0 );
	m_2DShader->Render( m_d3d->GetImmediateContext( ), m_DownSampleWindow->GetIndexCount( ), m_RenderTexture->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_DownSampleTexture->GetOrthoMatrix( ) );

	m_HorizontalBlurTexture->SetRenderTarget( m_d3d->GetImmediateContext( ) );
	m_HorizontalBlurTexture->BeginScene( m_d3d->GetImmediateContext( ), common::HexToRGB( 0x00FF00 ) );

	m_DownSampleWindow->Render( m_d3d->GetImmediateContext( ), 0, 0 );
	m_HorizontalBlur->Render( m_d3d->GetImmediateContext( ), m_DownSampleWindow->GetIndexCount( ), m_DownSampleTexture->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_DownSampleTexture->GetOrthoMatrix( ), m_HorizontalBlurTexture->GetTextureWidth( ) );

	m_VerticalBlurTexture->SetRenderTarget( m_d3d->GetImmediateContext( ) );
	m_VerticalBlurTexture->BeginScene( m_d3d->GetImmediateContext( ), common::HexToRGB( 0x00FF00 ) );

	m_DownSampleWindow->Render( m_d3d->GetImmediateContext( ), 0, 0 );
	m_VerticalBlur->Render( m_d3d->GetImmediateContext( ), m_DownSampleWindow->GetIndexCount( ), m_HorizontalBlurTexture->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_DownSampleTexture->GetOrthoMatrix( ), m_VerticalBlurTexture->GetTextureHeight( ) );

	m_UpSampleTexture->SetRenderTarget( m_d3d->GetImmediateContext( ) );
	m_UpSampleTexture->BeginScene( m_d3d->GetImmediateContext( ), common::HexToRGB( 0x00FF00 ) );
	
	m_UpSampleWindow->Render( m_d3d->GetImmediateContext( ), 0, 0 );
	m_2DShader->Render( m_d3d->GetImmediateContext( ), m_UpSampleWindow->GetIndexCount( ), m_VerticalBlurTexture->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_UpSampleTexture->GetOrthoMatrix( ) );

	m_d3d->EnableBackFaceCulling( );
	m_Floor->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Floor->GetIndexCount( ), m_Floor->GetTexture( ),
		m_Floor->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

	m_d3d->EnableBackBuffer( );
	m_d3d->BeginScene( );
	m_d3d->ResetViewPort( );

#pragma region Draw UI
	m_d3d->EnableAlphaBlending( );

	if ( RenderMenu )
	{
		if ( strlen( Cheat ) > 0 )
		{
			m_Cheat->Render( m_d3d->GetImmediateContext( ) );
			m_2DShader->Render( m_d3d->GetImmediateContext(), m_Cheat->GetIndexCount(), FontClass::GetTexture(),
				DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0xFFFF00 ) );
		}

		m_Cursor->Render( m_d3d->GetImmediateContext(), MouseX, MouseY );
		m_2DShader->Render( m_d3d->GetImmediateContext(), m_Cursor->GetIndexCount(), m_Cursor->GetTexture(),
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix() );
	}
	m_d3d->DisableAlphaBlending( );
#pragma endregion

	m_UpSampleWindow->Render( m_d3d->GetImmediateContext( ), 0, 0 );
	m_2DShader->Render( m_d3d->GetImmediateContext( ), m_UpSampleWindow->GetIndexCount( ), m_UpSampleTexture->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_UpSampleTexture->GetOrthoMatrix( ) );

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

	m_Floor->Shutdown( );
	delete m_Floor;
	
	m_RenderTexture->Shutdown( );
	delete m_RenderTexture;

	m_DownSampleTexture->Shutdown( );
	delete m_DownSampleTexture;

	m_HorizontalBlurTexture->Shutdown( );
	delete m_HorizontalBlurTexture;

	m_VerticalBlurTexture->Shutdown( );
	delete m_VerticalBlurTexture;

	m_UpSampleTexture->Shutdown( );
	delete m_UpSampleTexture;

	m_DownSampleWindow->Shutdown( );
	delete m_DownSampleWindow;

	m_UpSampleWindow->Shutdown( );
	delete m_UpSampleWindow;

	m_d3d->Shutdown();
	delete m_d3d;
}

CGraphics::~CGraphics()
{
	Shutdown();
}
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
	if (!m_d3d->Initialize( hInstance, hWnd, WindowWidth, WindowHeight, 0.1f, 100.0f ))
		return false;
	m_Cursor = new BitmapClass();
	if (!m_Cursor->Initialize( m_d3d->GetDevice(), L"data\\Cursor.dds", WindowWidth, WindowHeight, 32, 32 ))
		return false;
	m_RenderTexture = new BitmapClass();
	if (!m_RenderTexture->Initialize( m_d3d->GetDevice(), L"", WindowWidth, WindowHeight, WindowWidth / 10, WindowHeight / 10 ))
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
	m_2DShader = new C2DShader();
	if (!m_2DShader->Initialize( m_d3d->GetDevice() ))
		return false;
	m_Camera = new CCamera();
	if (!m_Camera->Initialize( DirectX::XMVectorSet( 0.0f, 0.0f, -10.0f, 1.0f ),
		0.5f * FLOAT_PI, (FLOAT)WindowWidth / WindowHeight, 0.1f, 100.0f, 15.0f ))
		return false;
	if (!FontClass::Initialize( m_d3d->GetDevice(), L"data\\font.dds", L"data\\fontdata.txt" ))
		return false;
	m_Cheat = new CSentence();
	if ( !m_Cheat->Initialize( m_d3d->GetDevice( ), "Here is a veeeery long string. Here will be wrote possible cheats",
		WindowWidth, WindowHeight, 1.0f, ( float ) WindowHeight - 17.0f ) )
		return false;
	m_CursorX = new CSentence( );
	if ( !m_CursorX->Initialize( m_d3d->GetDevice( ), "Cursor position X: 00000", WindowWidth, WindowHeight,
		WindowWidth - 150.0f, WindowHeight - 32.0f ) )
		return false;
	m_CursorY = new CSentence( );
	if ( !m_CursorY->Initialize( m_d3d->GetDevice( ), "Cursor position Y: 00000", WindowWidth, WindowHeight,
		WindowWidth - 150.0f, WindowHeight - 16.0f ) )
		return false;
	m_FPSMessage = new CSentence();
	if ( !m_FPSMessage->Initialize( m_d3d->GetDevice( ), "Frames per second: 000", WindowWidth, WindowHeight, 1.0f, 69.0f ) )
		return false;
	m_FrameTimeMessage = new CSentence();
	if ( !m_FrameTimeMessage->Initialize( m_d3d->GetDevice( ), "Frame time: 0.00", WindowWidth, WindowHeight, 1.0f, 20.0f ) )
		return false;
	m_TextureRenderer = new CRenderTexture();
	if (!m_TextureRenderer->Initialize( m_d3d->GetDevice(), WindowWidth, WindowHeight ))
		return false;
	m_ClippingPlane = DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
	Light = new CLight();
	Light->SetSpecularColor( common::HexToRGB( 0xFFFFFF ) );
	Light->SetAmbientColor( common::Color( 0.1f, 0.1f, 0.1f, 1.0f ) );
	Light->SetDiffuseColor( common::HexToRGB( 0xFFFFFF ) );
	Light->SetDirection( DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ) );
	Light->SetSpecularPower( 32.0f );


	m_fStartLoadingTime = 5;
	m_fLoadingTime = 0;
	m_bIncrease = true;
	return true;
}

void CGraphics::Update( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * cheat )
{
	static char MousePosition[ 4 ];
	static int index = 0;
	if (RenderMenu)
	{
		char buffer[500] = { 0 };
		sprintf_s( buffer, "Frames per second: %d", dwFramesPerSecond );
		m_FPSMessage->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), 1.0f, 1.0f );
		if (strcmp( cheat, "GetRenderCount" ) == 0)
		{
			sprintf_s( buffer, "Render count: %d", m_RenderCount );
			m_FrameTimeMessage->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ) - 1, 1.0f, 18.0f );
		}
		else
		{
			sprintf_s( buffer, "Frame time: %.2lf", fFrameTime );
			m_FrameTimeMessage->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), 1.0f, 18.0f );
		}
		if ( strlen( cheat ) > 0 )
		{
			sprintf_s( buffer, ":%s:", cheat );
			m_Cheat->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), 1.0f, ( float ) m_WindowHeight - 17.0f );
		}
		index = 0;
		memset( MousePosition, ( int )'0', sizeof( char ) * 4 );
		while ( MouseX && index < 4 )
		{
			MousePosition[ 3 - index++ ] = MouseX % 10 + '0';
			MouseX /= 10;
		}
		sprintf_s( buffer, "Cursor Position X: %s", MousePosition );
		m_CursorX->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), m_WindowWidth - 150.0f, m_WindowHeight - 32.0f );
		memset( MousePosition, ( int )'0', sizeof( char ) * 4 );
		index = 0;
		while ( MouseY && index < 4 )
		{
			MousePosition[ 3 - index++ ] = MouseY % 10 + '0';
			MouseY /= 10;
		}
		sprintf_s( buffer, "Cursor Position Y: %s", MousePosition );
		m_CursorY->Update( m_d3d->GetImmediateContext( ), buffer, strlen( buffer ), m_WindowWidth - 150.0f, m_WindowHeight - 16.0f );
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
	m_RenderCount = 0;
	m_Camera->Render();
	m_d3d->BeginScene( );
	m_d3d->DisableCulling( );

	if (RenderMenu)
	{

		m_d3d->EnableAlphaBlending();
		if ( strlen( Cheat ) > 0 )
		{
			m_Cheat->Render( m_d3d->GetImmediateContext( ) );
			m_2DShader->Render( m_d3d->GetImmediateContext(), m_Cheat->GetIndexCount(), FontClass::GetTexture(),
				DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0xFFFF00 ) );
		}

		m_FPSMessage->Render( m_d3d->GetImmediateContext( ) );
		m_2DShader->Render( m_d3d->GetImmediateContext(), m_FPSMessage->GetIndexCount(), FontClass::GetTexture(),
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0xFFFF00 ) );

		m_FrameTimeMessage->Render( m_d3d->GetImmediateContext( ) );
		m_2DShader->Render( m_d3d->GetImmediateContext(), m_FrameTimeMessage->GetIndexCount(), FontClass::GetTexture(),
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0x00FF00 ) );

		m_CursorX->Render( m_d3d->GetImmediateContext( ) );
		m_2DShader->Render( m_d3d->GetImmediateContext( ), m_CursorX->GetIndexCount( ), FontClass::GetTexture( ),
			DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_d3d->GetOrthoMatrix( ) );

		m_CursorY->Render( m_d3d->GetImmediateContext( ) );
		m_2DShader->Render( m_d3d->GetImmediateContext( ), m_CursorY->GetIndexCount( ), FontClass::GetTexture( ),
			DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_d3d->GetOrthoMatrix( ) );

		m_Cursor->Render( m_d3d->GetImmediateContext(), MouseX, MouseY );
		m_2DShader->Render( m_d3d->GetImmediateContext(), m_Cursor->GetIndexCount(), m_Cursor->GetTexture(),
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix() );
		m_d3d->DisableAlphaBlending();

	}


	m_d3d->EndScene();
}

void CGraphics::Shutdown()
{
	FontClass::Shutdown();

	m_TextureRenderer->Shutdown();
	delete m_RenderTexture;

	m_RenderTexture->Shutdown();
	delete m_RenderTexture;

	m_Cheat->Shutdown();
	delete m_Cheat;

	m_CursorX->Shutdown( );
	delete m_CursorX;

	m_CursorY->Shutdown( );
	delete m_CursorY;

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
	
	m_d3d->Shutdown();
	delete m_d3d;
}

CGraphics::~CGraphics()
{
	Shutdown();
}
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
	m_Triangle = new CModel();
	if (!m_Triangle->Initialize( m_d3d->GetDevice(), L"cube.txt", L"stone02.dds", L"bump02.dds", L"spec02.dds" ))
		return false;
	m_Model = new CModel();
	if (!m_Model->Initialize( m_d3d->GetDevice(), L"cube.txt", L"stone02.dds", L"bump02.dds", L"spec02.dds" ))
		return false;
	m_Cursor = new BitmapClass();
	if (!m_Cursor->Initialize( m_d3d->GetDevice(), L"Cursor.dds", WindowWidth, WindowHeight, 32, 32 ))
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
	m_LinearFogShader = new CFogShader();
	if (!m_LinearFogShader->Initialize(m_d3d->GetDevice(), CFogShader::EFogType::LinearFog))
		return false;
	m_ExponentialFogShader = new CFogShader();
	if (!m_ExponentialFogShader->Initialize(m_d3d->GetDevice(), CFogShader::EFogType::ExponentialFog))
		return false;
	m_ExponentialFogShader2 = new CFogShader();
	if (!m_ExponentialFogShader2->Initialize(m_d3d->GetDevice(), CFogShader::EFogType::ExponentialFog2))
		return false;
	m_2DShader = new C2DShader();
	if (!m_2DShader->Initialize( m_d3d->GetDevice() ))
		return false;
	m_Camera = new CCamera();
	if (!m_Camera->Initialize( DirectX::XMVectorSet( 0.0f, 0.0f, -7.0f, 1.0f ),
		0.5f * FLOAT_PI, (FLOAT)WindowWidth / WindowHeight, 0.1f, 100.0f, 15.0f ))
		return false;
	m_Up = new CCamera();
	if (!m_Up->Initialize( DirectX::XMVectorSet( 0.0f, 5.0f, -3.0f, 1.0f ),
		0.5f * FLOAT_PI, (FLOAT)WindowWidth / WindowHeight, 0.1f, 100.0f, 0.0f ))
		return false;
	m_Up->SetPitch( DEG2RAD( 70 ) );
	if (!FontClass::Initialize( m_d3d->GetDevice(), L"font.dds", L"fontdata.txt" ))
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
	return true;
}

void CGraphics::Update( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * cheat )
{
	static float delta = 1.0f;
	static char MousePosition[ 4 ];
	static int index;
	if ( m_ClippingPlane.w >= 1.1f || m_ClippingPlane.w <= -1.1f )
		delta *= -1;
	m_ClippingPlane.w += 0.5f * fFrameTime * delta;
	static float Rotation = 0.0f;
	if (Rotation >= 2 * FLOAT_PI)
		Rotation = 0.0f;
	Rotation += 0.125f * fFrameTime;
	m_Triangle->Identity();
	m_Triangle->RotateY( Rotation );
	m_Model->Identity();
	m_Model->Translate( 0.0f, 0.0f, 3.0f );
	m_Model->RotateY( 4 * Rotation );
	if (RenderMenu)
	{
		char buffer[500] = { 0 };
		sprintf_s( buffer, "Frames per second: %d", dwFramesPerSecond );
		m_FPSMessage->Update( m_d3d->GetImmediateContext( ), buffer, 1.0f, 1.0f );
		if (strcmp( cheat, "GetRenderCount" ) == 0)
		{
			sprintf_s( buffer, "Render count: %d", m_RenderCount );
			m_FrameTimeMessage->Update( m_d3d->GetImmediateContext( ), buffer, 1.0f, 18.0f );
		}
		else
		{
			sprintf_s( buffer, "Frame time: %.2lf", fFrameTime );
			m_FrameTimeMessage->Update( m_d3d->GetImmediateContext( ), buffer, 1.0f, 18.0f );
		}
		if ( strlen( cheat ) > 0 )
		{
			sprintf_s( buffer, ":%s:", cheat );
			m_Cheat->Update( m_d3d->GetImmediateContext( ), buffer, 1.0f, ( float ) m_WindowHeight - 17.0f );
		}
		index = 0;
		for ( int i = 0; i < 4; ++i )
			MousePosition[ i ] = '0';
		while ( MouseX && index < 4 )
		{
			MousePosition[ 3 - index++ ] = MouseX % 10 + '0';
			MouseX /= 10;
		}
		sprintf_s( buffer, "Cursor Position X: %s", MousePosition );
		m_CursorX->Update( m_d3d->GetImmediateContext( ), buffer, m_WindowWidth - 150.0f, m_WindowHeight - 32.0f );
		for ( int i = 0; i < 4; ++i )
			MousePosition[ i ] = '0';
		index = 0;
		while ( MouseY && index < 4 )
		{
			MousePosition[ 3 - index++ ] = MouseY % 10 + '0';
			MouseY /= 10;
		}
		sprintf_s( buffer, "Cursor Position Y: %s", MousePosition );
		m_CursorY->Update( m_d3d->GetImmediateContext( ), buffer, m_WindowWidth - 150.0f, m_WindowHeight - 16.0f );
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
	m_d3d->BeginScene();
	m_d3d->DisableCulling();
	m_Camera->Render();
	m_Up->Render();

	DirectX::XMVECTOR coord = DirectX::XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	coord = DirectX::XMVector3TransformCoord( coord, m_Triangle->GetWorld() );
	if (m_Camera->isCubeinFrustum( 1.0f, DirectX::XMVectorGetX( coord ), DirectX::XMVectorGetY( coord ), DirectX::XMVectorGetZ( coord ) ))
	{
		m_Triangle->Render( m_d3d->GetImmediateContext() );
		m_NoPlaneClippingShader->Render( m_d3d->GetImmediateContext( ), m_Triangle->GetIndexCount( ),
			m_Triangle->GetTexture( ), m_Triangle->GetBumpMap( ), m_Triangle->GetSpecularMap( ),
			m_Triangle->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), m_Camera->GetCameraPosition( ),
			Light, m_ClippingPlane );
		m_RenderCount++;
	}

	coord = DirectX::XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	coord = DirectX::XMVector3TransformCoord( coord, m_Model->GetWorld() );
	if (m_Camera->isSphereinFrustum( 1.0f, DirectX::XMVectorGetX( coord ), DirectX::XMVectorGetY( coord ), DirectX::XMVectorGetZ( coord ) ))
	{
		m_Model->Render( m_d3d->GetImmediateContext() );
		m_NoPlaneClippingShader->Render( m_d3d->GetImmediateContext( ), m_Model->GetIndexCount( ),
			m_Model->GetTexture( ), m_Model->GetBumpMap( ), m_Model->GetSpecularMap( ),
			m_Model->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), m_Camera->GetCameraPosition( ),
			Light, m_ClippingPlane );
		m_RenderCount++;
	}

	m_TextureRenderer->SetRenderTarget( m_d3d->GetImmediateContext(), m_d3d->GetDepthStencilView() );
	m_TextureRenderer->BeginScene( m_d3d->GetImmediateContext( ), m_d3d->GetDepthStencilView( ), common::Color( 0.5f, 0.5f, 0.5f, 0.5f ) );

	m_Triangle->Render( m_d3d->GetImmediateContext() );
	m_NoPlaneClippingShader->Render( m_d3d->GetImmediateContext(), m_Triangle->GetIndexCount(),
		m_Triangle->GetTexture(), m_Triangle->GetBumpMap(), m_Triangle->GetSpecularMap(),
		m_Triangle->GetWorld(), m_Up->GetView(), m_Up->GetProjection(), m_Up->GetCameraPosition(),
		Light );

	m_Model->Render( m_d3d->GetImmediateContext() );
	m_NoPlaneClippingShader->Render( m_d3d->GetImmediateContext(), m_Model->GetIndexCount(),
		m_Model->GetTexture(), m_Model->GetBumpMap(), m_Model->GetSpecularMap(),
		m_Model->GetWorld(), m_Up->GetView(), m_Up->GetProjection(), m_Up->GetCameraPosition(),
		Light );

	m_d3d->EnableBackBuffer();

	m_RenderTexture->Render( m_d3d->GetImmediateContext(), 50, 50 );
	m_2DShader->Render( m_d3d->GetImmediateContext(), m_RenderTexture->GetIndexCount(), m_TextureRenderer->GetTexture(),
		DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix() );

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

	m_LinearFogShader->Shutdown();
	delete m_LinearFogShader;

	m_ExponentialFogShader->Shutdown();
	delete m_ExponentialFogShader;

	m_ExponentialFogShader2->Shutdown();
	delete m_ExponentialFogShader2;

	delete Light;

	m_Up->Shutdown();
	delete m_Up;
	
	m_Camera->Shutdown();
	delete m_Camera;
	
	m_Cursor->Shutdown();
	delete m_Cursor;

	m_Model->Shutdown();
	delete m_Model;

	m_Triangle->Shutdown();
	delete m_Triangle;
	
	m_d3d->Shutdown();
	delete m_d3d;
}

CGraphics::~CGraphics()
{
	Shutdown();
}
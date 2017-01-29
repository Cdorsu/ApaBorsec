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
	m_3DSimpleShader = new CSimpleShader();
	if (!m_3DSimpleShader->Initialize( m_d3d->GetDevice() ))
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
	if (!m_Cheat->Initialize( m_d3d->GetDevice(), "Cheat message", WindowWidth, WindowHeight ))
		return false;
	m_FPSMessage = new CSentence();
	if (!m_FPSMessage->Initialize( m_d3d->GetDevice(), "FPSMessage", WindowWidth, WindowHeight ))
		return false;
	m_FrameTimeMessage = new CSentence();
	if (!m_FrameTimeMessage->Initialize( m_d3d->GetDevice(), "FTMessage", WindowWidth, WindowHeight ))
		return false;
	m_CursorPosition = new CSentence();
	if (!m_CursorPosition->Initialize( m_d3d->GetDevice(), "CursorPositionX\nCursorPositionY", WindowWidth, WindowHeight ))
		return false;
	m_TextureRenderer = new CRenderTexture();
	if (!m_TextureRenderer->Initialize( m_d3d->GetDevice(), WindowWidth, WindowHeight ))
		return false;
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
	m_MouseX = MouseX;
	m_MouseY = MouseY;
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
		m_FPSMessage->Update( m_d3d->GetDevice(), buffer );
		if (strcmp( cheat, "GetRenderCount" ) == 0)
		{
			sprintf_s( buffer, "Render count: %d", m_RenderCount );
			m_FrameTimeMessage->Update( m_d3d->GetDevice(), buffer );
		}
		else
		{
			sprintf_s( buffer, "Frame time: %.2lf", fFrameTime );
			m_FrameTimeMessage->Update( m_d3d->GetDevice(), buffer );
		}
		sprintf_s( buffer, "CursorPositionX: %d\nCursorPositionY: %d", MouseX, MouseY );
		m_CursorPosition->Update( m_d3d->GetDevice(), buffer );
		sprintf_s( buffer, ":%s:", cheat );
		m_Cheat->Update( m_d3d->GetDevice(), buffer );
	}
	else
	{
		m_Camera->Update( fFrameTime );
	}

}

void CGraphics::Frame( bool RenderMenu, DWORD dwFramesPerSecond, float fFrameTime, UINT MouseX, UINT MouseY, char * Cheat )
{
	Update( RenderMenu, dwFramesPerSecond, fFrameTime, MouseX, MouseY, Cheat );
	Render( RenderMenu, Cheat );
}

void CGraphics::Render( bool RenderMenu, char * Cheat )
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
		m_ExponentialFogShader2->Render(m_d3d->GetImmediateContext(), m_Triangle->GetIndexCount(),
			m_Triangle->GetTexture(), m_Triangle->GetBumpMap(), m_Triangle->GetSpecularMap(),
			m_Triangle->GetWorld(), m_Camera->GetView(), m_Camera->GetProjection(), m_Camera->GetCameraPosition(),
			Light, 0.0f, 100.0f, common::Color(0.5f, 0.5f, 0.5f, 1.0f));
		m_RenderCount++;
	}

	coord = DirectX::XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	coord = DirectX::XMVector3TransformCoord( coord, m_Model->GetWorld() );
	if (m_Camera->isSphereinFrustum( 1.0f, DirectX::XMVectorGetX( coord ), DirectX::XMVectorGetY( coord ), DirectX::XMVectorGetZ( coord ) ))
	{
		m_Model->Render( m_d3d->GetImmediateContext() );
		m_LinearFogShader->Render(m_d3d->GetImmediateContext(), m_Model->GetIndexCount(),
			m_Model->GetTexture(), m_Model->GetBumpMap(), m_Model->GetSpecularMap(),
			m_Model->GetWorld(), m_Camera->GetView(), m_Camera->GetProjection(), m_Camera->GetCameraPosition(),
			Light, 0.0f, 100.0f, common::Color(0.5f, 0.5f, 0.5f, 1.0f));
		m_RenderCount++;
	}

	m_TextureRenderer->SetRenderTarget( m_d3d->GetImmediateContext(), m_d3d->GetDepthStencilView() );
	m_TextureRenderer->BeginScene( m_d3d->GetImmediateContext(), m_d3d->GetDepthStencilView(), common::HexToRGB( 0x000000 ) );

	m_Triangle->Render( m_d3d->GetImmediateContext() );
	m_3DSimpleShader->Render( m_d3d->GetImmediateContext(), m_Triangle->GetIndexCount(),
		m_Triangle->GetTexture(), m_Triangle->GetBumpMap(), m_Triangle->GetSpecularMap(),
		m_Triangle->GetWorld(), m_Up->GetView(), m_Up->GetProjection(), m_Up->GetCameraPosition(),
		Light->GetAmbientColor(), Light->GetDiffuseColor(), Light->GetSpecularColor(), Light->GetDirection(),
		Light->GetSpecularPower() );

	m_Model->Render( m_d3d->GetImmediateContext() );
	m_3DSimpleShader->Render( m_d3d->GetImmediateContext(), m_Model->GetIndexCount(),
		m_Model->GetTexture(), m_Model->GetBumpMap(), m_Model->GetSpecularMap(),
		m_Model->GetWorld(), m_Up->GetView(), m_Up->GetProjection(), m_Up->GetCameraPosition(),
		Light->GetAmbientColor(), Light->GetDiffuseColor(), Light->GetSpecularColor(), Light->GetDirection(),
		Light->GetSpecularPower() );

	m_d3d->EnableBackBuffer();

	m_RenderTexture->Render( m_d3d->GetImmediateContext(), 50, 50 );
	m_2DShader->Render( m_d3d->GetImmediateContext(), m_RenderTexture->GetIndexCount(), m_TextureRenderer->GetTexture(),
		DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix() );

	if (RenderMenu)
	{

		m_d3d->EnableAlphaBlending();
		if (Cheat != "")
		{
			m_Cheat->Render( m_d3d->GetImmediateContext(), 1.0f, m_WindowHeight - 17.0f );
			m_2DShader->Render( m_d3d->GetImmediateContext(), m_Cheat->GetIndexCount(), FontClass::GetTexture(),
				DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0xFFFF00 ) );
		}

		m_FPSMessage->Render( m_d3d->GetImmediateContext(), 1, 1 );
		m_2DShader->Render( m_d3d->GetImmediateContext(), m_FPSMessage->GetIndexCount(), FontClass::GetTexture(),
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0xFFFF00 ) );

		m_FrameTimeMessage->Render( m_d3d->GetImmediateContext(), 1, 20 );
		m_2DShader->Render( m_d3d->GetImmediateContext(), m_FrameTimeMessage->GetIndexCount(), FontClass::GetTexture(),
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0x00FF00 ) );

		m_CursorPosition->Render( m_d3d->GetImmediateContext(), m_WindowWidth - 150.0f, m_WindowHeight - 32.0f );
		m_2DShader->Render( m_d3d->GetImmediateContext(), m_CursorPosition->GetIndexCount(), FontClass::GetTexture(),
			DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), m_d3d->GetOrthoMatrix(), common::HexToRGB( 0xFF0000 ) );

		m_Cursor->Render( m_d3d->GetImmediateContext(), m_MouseX, m_MouseY );
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

	m_CursorPosition->Shutdown();
	delete m_CursorPosition;

	m_FrameTimeMessage->Shutdown();
	delete m_FrameTimeMessage;

	m_FPSMessage->Shutdown();
	delete m_FPSMessage;

	m_2DShader->Shutdown();
	delete m_2DShader;

	m_3DSimpleShader->Shutdown();
	delete m_3DSimpleShader;

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
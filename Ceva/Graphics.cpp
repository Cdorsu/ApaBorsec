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
	if ( !m_DebugWindow->Initialize( m_d3d->GetDevice( ), L"", WindowWidth, WindowHeight, 0.5f * WindowWidth, 0.5f * WindowHeight ) )
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
	m_ColorShadowShader = new CShadowShader( );
	if ( !m_ColorShadowShader->Initialize( m_d3d->GetDevice( ), CShadowShader::EType::Color ) )
		return false;
	m_BWShadowShader = new CShadowShader( );
	if ( !m_BWShadowShader->Initialize( m_d3d->GetDevice( ), CShadowShader::EType::BlackWhite ) )
		return false;
	m_MultipleShadowShader = new CMultipleShadowShader( );
	if ( !m_MultipleShadowShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_SoftShadowShader = new CSoftShadowShader();
	if (!m_SoftShadowShader->Initialize(m_d3d->GetDevice()))
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
	m_ProjectionShader = new CProjectionShader( );
	if ( !m_ProjectionShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_ProjectiveLightShader = new CProjectiveLightShader( );
	if ( !m_ProjectiveLightShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_MaskShader = new CMaskShader( );
	if ( !m_MaskShader->Initialize( m_d3d->GetDevice( ) ) )
		return false;
	m_CombineTextureShader = new CCombineTextureShader( );
	if ( !m_CombineTextureShader->Initialize( m_d3d->GetDevice( ) ) )
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
	m_Button = new BitmapClass( );
	if ( !m_Button->Initialize( m_d3d->GetDevice( ), L"", WindowWidth, WindowHeight, ButtonWidth, ButtonHeight ) )
		return false;
	m_GlowTest = new CGlow( );
	if ( !m_GlowTest->Initialize( m_d3d->GetDevice( ), L"data\\test.dds", L"data\\glowmap.dds",
		m_HorizontalBlur, m_VerticalBlur, m_2DShader, m_MaskShader, m_CombineTextureShader,
		m_WindowWidth, m_WindowHeight, ButtonWidth, ButtonHeight, FOV, camNear, camFar ) )
		return false;
	m_Sphere = new CModel( );
	if ( !m_Sphere->Initialize( m_d3d->GetDevice( ), L"data\\sphere.txt", L"data\\marble01.dds" ) )
		return false;
	m_Sphere->Translate( 0.0f, 2.0f, -10.0f );
	m_Floor = new CModel( );
	if ( !m_Floor->Initialize( m_d3d->GetDevice( ), L"data\\floor.txt", L"data\\checkboard.dds" ) )
		return false;
	m_Floor->Translate( 0.f, 0.f, -10.f );
	m_Wall = new CModel( );
	if ( !m_Wall->Initialize( m_d3d->GetDevice( ), L"data\\floor.txt", L"data\\brick01.dds" ) )
		return false;
	m_Wall->RotateX( -FLOAT_PI / 2 );
	m_Wall->Translate( 0.0f, 0.0f, 5.0f );
	m_Mirror = new CModel( );
	if ( !m_Mirror->Initialize( m_d3d->GetDevice( ), L"data\\floor.txt", L"data\\blue01.dds" ) )
		return false;
	m_Mirror->RotateX( -FLOAT_PI / 2 );
	m_Mirror->Translate( 0.0f, 5.0f, 0.0f );
	m_Mirror->Scale( 0.5f, 0.5f, 0.5f );

	Light = new CLight();
	Light->SetSpecularColor( common::HexToRGB( 0xFFFFFF ) );
	Light->SetAmbientColor( common::Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
	Light->SetDiffuseColor( common::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
	Light->SetDirection( DirectX::XMFLOAT3( 1.0f, -1.0f, 1.0f ) );
	Light->SetSpecularPower( 32.0f );

	PointLight = new CPointLight( );
	PointLight->SetPosition( DirectX::XMFLOAT3( 0.0f, 3.0f, -10.0f ) );
	PointLight->SetDiffuseColor( common::HexToRGB( 0x0000FF ) );
	PointLight->SetAttenuation( 0.4f, 0.2f, 0.0f );
	PointLight->SetRange( 0 ); // disable point light

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

	Update( RenderMenu, dwFramesPerSecond, fFrameTime, MouseX, MouseY, Cheat );
	Render( RenderMenu, Cheat, MouseX, MouseY );

}

void CGraphics::Render( bool RenderMenu, char * Cheat, UINT MouseX, UINT MouseY )
{
	m_Camera->Render();
	m_d3d->DisableCulling( );

	m_d3d->EnableBackBuffer( );
	m_d3d->ResetViewPort( );
	m_d3d->BeginScene( );

	//m_Wall->Render( m_d3d->GetImmediateContext( ) );
	//m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Wall->GetIndexCount( ), m_Wall->GetTexture( ),
		//m_Wall->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

	m_Floor->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Floor->GetIndexCount( ), m_Floor->GetTexture( ),
		m_Floor->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

	m_Sphere->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Sphere->GetIndexCount( ), m_Sphere->GetTexture( ),
		m_Sphere->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );

	m_d3d->DisableRenderTargetWrite( );
	m_d3d->WriteToStencil( );
	m_Mirror->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Mirror->GetIndexCount( ), m_Mirror->GetTexture( ),
		m_Mirror->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );
	m_d3d->DisableSkyRendering( );
	m_d3d->DisableAlphaBlending( );


	m_d3d->DisableCulling( );
	m_d3d->EnableMirrorRendering( );
	DirectX::XMMATRIX R = DirectX::XMMatrixReflect( DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ) );
	m_Sphere->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Sphere->GetIndexCount( ), m_Sphere->GetTexture( ),
		m_Sphere->GetWorld( ) * R, m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );
	m_d3d->DisableSkyRendering( );
	m_d3d->EnableBackFaceCulling( );

	m_d3d->EnableTransparency( );
	m_Mirror->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Mirror->GetIndexCount( ), m_Mirror->GetTexture( ),
		 m_Mirror->GetWorld( ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );
	m_d3d->DisableAlphaBlending( );

	m_d3d->EnableShadowRendering( );
	DirectX::XMVECTOR Plane = DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	DirectX::XMVECTOR LightPos = DirectX::XMVectorSet( 0.0f, 9.0f, 0.0f, 1.0f );
	DirectX::XMMATRIX Shadow = DirectX::XMMatrixShadow( Plane, LightPos );
	m_Sphere->Render( m_d3d->GetImmediateContext( ) );
	m_TextureShader->Render( m_d3d->GetImmediateContext( ), m_Sphere->GetIndexCount( ), m_Sphere->GetTexture( ),
		m_Sphere->GetWorld( ) * Shadow * DirectX::XMMatrixTranslation( 0.0f, 0.001f, 0.0f ), m_Camera->GetView( ), m_Camera->GetProjection( ), Light, PointLight );
	m_d3d->DisableSkyRendering( );

#pragma region Draw UI
	//m_d3d->EnableAlphaBlending( );

	if ( RenderMenu )
	{
		if ( strlen( Cheat ) > 0 )
		{
			m_Cheat->Render( m_d3d->GetImmediateContext( ) );
			m_2DShader->Render( m_d3d->GetImmediateContext( ), m_Cheat->GetIndexCount( ), FontClass::GetTexture( ),
				DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_d3d->GetOrthoMatrix( ), common::HexToRGB( 0xFFFF00 ) );
		}

		m_FPSMessage->Render( m_d3d->GetImmediateContext( ) );
		m_2DShader->Render( m_d3d->GetImmediateContext( ), m_FPSMessage->GetIndexCount( ), FontClass::GetTexture( ),
			DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_d3d->GetOrthoMatrix( ), common::HexToRGB( 0xFFFF00 ) );

		m_FrameTimeMessage->Render( m_d3d->GetImmediateContext( ) );
		m_2DShader->Render( m_d3d->GetImmediateContext( ), m_FrameTimeMessage->GetIndexCount( ), FontClass::GetTexture( ),
			DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_d3d->GetOrthoMatrix( ), common::HexToRGB( 0xFFFF00 ) );

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
	
	m_Sphere->Shutdown( );
	delete m_Sphere;
	
	m_Wall->Shutdown( );
	delete m_Wall;

	m_Floor->Shutdown( );
	delete m_Floor;

	m_Mirror->Shutdown( );
	delete m_Mirror;

	m_Cheat->Shutdown();
	delete m_Cheat;

	m_GlowTest->Shutdown( );
	delete m_GlowTest;

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

	m_ColorShadowShader->Shutdown( );
	delete m_ColorShadowShader;

	m_BWShadowShader->Shutdown( );
	delete m_BWShadowShader;

	m_MultipleShadowShader->Shutdown( );
	delete m_MultipleShadowShader;

	m_SoftShadowShader->Shutdown();
	delete m_SoftShadowShader;

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

	m_MaskShader->Shutdown( );
	delete m_MaskShader;

	m_CombineTextureShader->Shutdown( );
	delete m_CombineTextureShader;

	delete Light;
	delete PointLight;

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

	m_ProjectionShader->Shutdown( );
	delete m_InstanceShader;

	m_ProjectiveLightShader->Shutdown( );
	delete m_ProjectiveLightShader;

	m_Button->Shutdown( );
	delete m_Button;

	m_d3d->Shutdown();
	delete m_d3d;
}

CGraphics::~CGraphics()
{
	Shutdown();
}
#include "Glow.h"

CGlow::CGlow( )
{
	ZeroMemory( this, sizeof( CGlow ) );
}

bool CGlow::Initialize( ID3D11Device * device, LPWSTR lpTexture, LPWSTR lpGlowmap,
	CHorizontalBlurShader * HorizontalBlurShader, CVerticalBlurShader * VerticalBlurShader,
	C2DShader * twShader, CMaskShader * MaskShader, CCombineTextureShader * CombineTexture,
	UINT WindowWidth, UINT WindowHeight,
	UINT ButtonWidth, UINT ButtonHeight, float FOV, float camNear, float camFar )
{
	m_CombineTextureShader = CombineTexture;
	m_2DShader = twShader;
	m_MaskShader = MaskShader;
	m_HorizontalBlurShader = HorizontalBlurShader; // copy the address, not the object
	m_VerticalBlurShader = VerticalBlurShader;
	m_Texture = new CTexture( );
	if ( !m_Texture->Initialize( device, lpTexture ) )
		return false;
	m_Glowmap = new CTexture( );
	if ( !m_Glowmap->Initialize( device, lpGlowmap ) )
		return false;
	m_ToGlow = new CRenderTexture( );
	if ( !m_ToGlow->Initialize( device, ButtonWidth, ButtonHeight, FOV, camNear, camFar ) )
		return false;
	UINT downSampleWidth = ButtonWidth / 2;
	UINT downSampleHeight = ButtonHeight / 2;
	m_DownSampleTexture = new CRenderTexture( );
	if ( !m_DownSampleTexture->Initialize( device, downSampleWidth, downSampleHeight, FOV, camNear, camFar ) )
		return false;
	m_HorizontalBlurTexture = new CRenderTexture( );
	if ( !m_HorizontalBlurTexture->Initialize( device, downSampleWidth, downSampleHeight, FOV, camNear, camFar ) )
		return false;
	m_VerticalBlurTexture = new CRenderTexture( );
	if ( !m_VerticalBlurTexture->Initialize( device, downSampleWidth, downSampleHeight, FOV, camNear, camFar ) )
		return false;
	m_CombinedTextures = new CRenderTexture( );
	if ( !m_CombinedTextures->Initialize( device, ButtonWidth, ButtonHeight, FOV, camNear, camFar ) )
		return false;
	m_Button = new BitmapClass( );
	if ( !m_Button->Initialize( device, L"", ButtonWidth, ButtonHeight, ButtonWidth, ButtonHeight ) )
		return false;
	m_DownSampleButton = new BitmapClass( );
	if ( !m_DownSampleButton->Initialize( device, L"",
		downSampleWidth, downSampleHeight, downSampleWidth, downSampleHeight ) )
		return false;
	m_WindowWidth = WindowWidth;
	m_WindowHeight = WindowHeight;
	return true;
}

void CGlow::Render( ID3D11DeviceContext * context )
{
	m_ToGlow->SetRenderTarget( context );
	m_ToGlow->BeginScene( context, common::HexToRGB( 0xFFFFFF ) );

	m_Button->Render( context, 0, 0 );
	m_MaskShader->Render( context, m_Button->GetIndexCount( ), m_Texture->GetTexture( ),
		m_Glowmap->GetTexture( ), DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ),
		m_ToGlow->GetOrthoMatrix( ) );

	m_DownSampleTexture->SetRenderTarget( context );
	m_DownSampleTexture->BeginScene( context, common::HexToRGB( 0x0 ) );

	m_DownSampleButton->Render( context, 0, 0 );
	m_2DShader->Render( context, m_DownSampleButton->GetIndexCount( ), m_ToGlow->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_DownSampleTexture->GetOrthoMatrix( ) );

	m_HorizontalBlurTexture->SetRenderTarget( context );
	m_HorizontalBlurTexture->BeginScene( context, common::HexToRGB( 0x0 ) );

	m_DownSampleButton->Render( context, 0, 0 );
	m_HorizontalBlurShader->Render( context, m_DownSampleButton->GetIndexCount( ), m_DownSampleTexture->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_HorizontalBlurTexture->GetOrthoMatrix( ),
		( FLOAT ) m_WindowWidth );

	m_VerticalBlurTexture->SetRenderTarget( context );
	m_VerticalBlurTexture->BeginScene( context, common::HexToRGB( 0x0 ) );

	m_DownSampleButton->Render( context, 0, 0 );
	m_VerticalBlurShader->Render( context, m_DownSampleButton->GetIndexCount( ), m_HorizontalBlurTexture->GetTexture( ),
		DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ), m_HorizontalBlurTexture->GetOrthoMatrix( ),
		( FLOAT ) m_WindowHeight );

	m_CombinedTextures->SetRenderTarget( context );
	m_CombinedTextures->BeginScene( context, common::HexToRGB( 0x0 ) );

	m_Button->Render( context, 0, 0 );
	m_CombineTextureShader->Render( context, m_Button->GetIndexCount( ), m_Texture->GetTexture( ),
		m_VerticalBlurTexture->GetTexture( ), DirectX::XMMatrixIdentity( ), DirectX::XMMatrixIdentity( ),
		m_CombinedTextures->GetOrthoMatrix( ), 3.0f );
}

ID3D11ShaderResourceView* CGlow::GetTexture( )
{
	return m_CombinedTextures->GetTexture( );
}

void CGlow::Shutdown( )
{
	m_ToGlow->Shutdown( );
	delete m_Texture;

	m_Texture->Shutdown( );
	delete m_Texture;

	m_Glowmap->Shutdown( );
	delete m_Glowmap;

	m_Button->Shutdown( );
	delete m_Button;
	
	m_DownSampleButton->Shutdown( );
	delete m_DownSampleButton;

	m_DownSampleTexture->Shutdown( );
	delete m_DownSampleTexture;

	m_HorizontalBlurTexture->Shutdown( );
	delete m_HorizontalBlurTexture;

	m_VerticalBlurTexture->Shutdown( );
	delete m_VerticalBlurTexture;
	
	m_CombinedTextures->Shutdown( );
	delete m_CombinedTextures;
}

CGlow::~CGlow( )
{
	Shutdown( );
}
#include "Game.h"


CGame::CGame()
{
	ZeroMemory( this, sizeof( this ) );
}

bool CGame::Initialize( HINSTANCE hInstance )
{
	m_hInstance = hInstance;
	if (!InitializeWindow())
		return false;
	if (!CInput::Initialize( hInstance, m_hWnd, m_WindowWidth, m_WindowHeight ))
		return false;
	INPUT_INSTANCE->AddSpecialKey( DIK_RETURN );
	INPUT_INSTANCE->AddSpecialKey( DIK_BACKSPACE );

	INPUT_INSTANCE->AddSpecialKey( DIK_A );
	INPUT_INSTANCE->AddSpecialKey( DIK_B );
	INPUT_INSTANCE->AddSpecialKey( DIK_C );
	INPUT_INSTANCE->AddSpecialKey( DIK_D );
	INPUT_INSTANCE->AddSpecialKey( DIK_E );
	INPUT_INSTANCE->AddSpecialKey( DIK_F );
	INPUT_INSTANCE->AddSpecialKey( DIK_G );
	INPUT_INSTANCE->AddSpecialKey( DIK_H );
	INPUT_INSTANCE->AddSpecialKey( DIK_I );
	INPUT_INSTANCE->AddSpecialKey( DIK_J );
	INPUT_INSTANCE->AddSpecialKey( DIK_K );
	INPUT_INSTANCE->AddSpecialKey( DIK_L );
	INPUT_INSTANCE->AddSpecialKey( DIK_M );
	INPUT_INSTANCE->AddSpecialKey( DIK_N );
	INPUT_INSTANCE->AddSpecialKey( DIK_O );
	INPUT_INSTANCE->AddSpecialKey( DIK_P );
	INPUT_INSTANCE->AddSpecialKey( DIK_Q );
	INPUT_INSTANCE->AddSpecialKey( DIK_R );
	INPUT_INSTANCE->AddSpecialKey( DIK_S );
	INPUT_INSTANCE->AddSpecialKey( DIK_T );
	INPUT_INSTANCE->AddSpecialKey( DIK_U );
	INPUT_INSTANCE->AddSpecialKey( DIK_V );
	INPUT_INSTANCE->AddSpecialKey( DIK_W );
	INPUT_INSTANCE->AddSpecialKey( DIK_X );
	INPUT_INSTANCE->AddSpecialKey( DIK_Y );
	INPUT_INSTANCE->AddSpecialKey( DIK_Z );
	INPUT_INSTANCE->AddSpecialKey( DIK_SPACE );


	m_Graphics = new CGraphics();
	if (!m_Graphics->Initialize( hInstance, m_hWnd, m_WindowWidth, m_WindowHeight ))
		return false;
	return true;
}

bool CGame::InitializeWindow()
{
	// Describe and register window class
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof( WNDCLASSEX );
	wndClass.hbrBackground = (HBRUSH)(GetStockObject( DKGRAY_BRUSH ));
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndClass.hIcon = LoadIcon( m_hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
	wndClass.hInstance = m_hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = L"Game";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClassEx( &wndClass ))
		return false;
	m_WindowWidth = GetSystemMetrics( SM_CXSCREEN );
	m_WindowHeight = GetSystemMetrics( SM_CYSCREEN );
	// Create, show and update window
	m_hWnd = CreateWindowEx( WS_EX_APPWINDOW, L"Game", L"Engine",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
		m_WindowWidth, m_WindowHeight, NULL, NULL, m_hInstance, NULL );
	if (!m_hWnd)
		return false;

	ShowWindow( m_hWnd, SW_SHOWNORMAL );
	SetFocus( m_hWnd );
	UpdateWindow( m_hWnd );

	return true;
}

void CGame::HandleKeyInput()
{
	if (m_bShowMenu)
	{
		std::vector<char> pressed = INPUT_INSTANCE->AlphaPressed();
		for (UINT i = 0; i < pressed.size(); ++i)
			m_Cheat.operator+=( pressed[i] );
		if (INPUT_INSTANCE->isSpecialKeyPressed( DIK_BACKSPACE ))
		{
			if (m_Cheat.size() > 0)
				m_Cheat.erase( m_Cheat.end() - 1, m_Cheat.end() );
		}
	}
	if (INPUT_INSTANCE->isSpecialKeyPressed( DIK_RETURN ))
	{
		m_bShowMenu = m_bShowMenu ? false : true;
		m_Cheat.clear();
	}
}

void CGame::Run()
{
	m_HRTimer.StartTimer();
	MSG Message;
	while (true)
	{
		if (PeekMessage( &Message, 0, 0, 0, PM_REMOVE ))
		{
			if (Message.message == WM_QUIT)
				break;
			TranslateMessage( &Message );
			DispatchMessage( &Message );
		}
		else
		{
			INPUT_INSTANCE->Frame();
			if (INPUT_INSTANCE->isKeyPressed( DIK_ESCAPE ))
				break;
			HandleKeyInput();
			if (m_HRTimer.GetTime() > 1.0f)
				m_HRTimer.StartTimer();
			m_HRTimer.Frame();
			m_Graphics->Frame( m_bShowMenu, m_HRTimer.GetFPS(), m_HRTimer.GetFrameTime(),
				INPUT_INSTANCE->GetCursorX(), INPUT_INSTANCE->GetCursorY(),
				const_cast<char*>(m_Cheat.c_str()) );
		}
	}
}

void CGame::Shutdown()
{
	delete m_Graphics;
	CInput::Destroy();
	UnregisterClass( L"Game", m_hInstance );
	DestroyWindow( m_hWnd );
}

CGame::~CGame()
{
	Shutdown();
}

LRESULT CALLBACK CGame::WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch (Message)
	{
	case WM_QUIT:
		DestroyWindow( hWnd );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}
	return DefWindowProc( hWnd, Message, wParam, lParam );
}
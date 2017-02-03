#include "Input.h"

CInput* CInput::m_Instance = nullptr;

CInput::CInput( HINSTANCE hInstance, HWND hWnd, UINT ScreenWidth, UINT ScreenHeight )
{
	m_ScreenWidth = (LONG)ScreenWidth;
	m_ScreenHeight = (LONG)ScreenHeight;
	m_CursorPosition = { 0,0 };
	HRESULT hr;
	hr = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&m_DIDevice, NULL );
	if (FAILED( hr ))
		throw std::exception( "Couldn't create direct input device" );
	hr = m_DIDevice->CreateDevice( GUID_SysKeyboard, &m_DIKeyboard, NULL );
	if (FAILED( hr ))
		throw std::exception( "Couldn't create keyboard direct input device" );
	hr = m_DIDevice->CreateDevice( GUID_SysMouse, &m_DIMouse, NULL );
	if (FAILED( hr ))
		throw std::exception( "Couldn't create mouse direct input device" );
	hr = m_DIKeyboard->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND );
	if (FAILED( hr ))
		throw std::exception( "Couldn't set cooperative level to mouse" );
	hr = m_DIKeyboard->SetDataFormat( &c_dfDIKeyboard );
	if (FAILED( hr ))
		throw std::exception( "Couldn't set data format" );
	hr = m_DIMouse->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND );
	if (FAILED( hr ))
		throw std::exception( "Couldn't set cooperative level to mouse" );
	hr = m_DIMouse->SetDataFormat( &c_dfDIMouse );
	if (FAILED( hr ))
		throw std::exception( "Couldn't set data format" );
}

void CInput::Frame()
{
	for (UINT i = 0; i < m_specialkeys.size(); ++i)
		m_lastkeyboard[m_specialkeys[i]] = m_currentkeyboard[m_specialkeys[i]];
	m_lastMouseState = m_currentMouseState;
	static HRESULT hr;
	m_DIKeyboard->Acquire();
	m_DIMouse->Acquire();

	m_DIMouse->GetDeviceState( sizeof( DIMOUSESTATE ), &m_currentMouseState );
	m_DIKeyboard->GetDeviceState( sizeof( m_currentkeyboard ), m_currentkeyboard );
	
	m_CursorPosition.x += m_currentMouseState.lX;
	m_CursorPosition.y += m_currentMouseState.lY;

	if (m_CursorPosition.x < 0)
		m_CursorPosition.x = 0;
	else if (m_CursorPosition.x > m_ScreenWidth)
		m_CursorPosition.x = m_ScreenWidth;
	if (m_CursorPosition.y < 0)
		m_CursorPosition.y = 0;
	else if (m_CursorPosition.y > m_ScreenHeight)
		m_CursorPosition.y = m_ScreenHeight;

}

std::vector<char> CInput::AlphaPressed()
{
	std::vector<char> v;
	for (int i = 0; i < 256; ++i)
	{
		if (i == DIK_A && isSpecialKeyPressed( DIK_A ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'A' );
			else
				v.push_back( 'a' );
		}
		else if (i == DIK_B && isSpecialKeyPressed( DIK_B ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'B' );
			else
				v.push_back( 'b' );
		}
		else if (i == DIK_C && isSpecialKeyPressed( DIK_C ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'C' );
			else
				v.push_back( 'c' );
		}
		else if (i == DIK_D && isSpecialKeyPressed( DIK_D ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'D' );
			else
				v.push_back( 'd' );
		}
		else if (i == DIK_E && isSpecialKeyPressed( DIK_E ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'E' );
			else
				v.push_back( 'e' );
		}
		else if (i == DIK_F && isSpecialKeyPressed( DIK_F ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'F' );
			else
				v.push_back( 'f' );
		}
		else if (i == DIK_G && isSpecialKeyPressed( DIK_G ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'G' );
			else
				v.push_back( 'g' );
		}
		else if (i == DIK_H && isSpecialKeyPressed( DIK_H ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'H' );
			else
				v.push_back( 'h' );
		}
		else if (i == DIK_I && isSpecialKeyPressed( DIK_I ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'I' );
			else
				v.push_back( 'i' );
		}
		else if (i == DIK_J && isSpecialKeyPressed( DIK_J ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'J' );
			else
				v.push_back( 'j' );
		}
		else if (i == DIK_K && isSpecialKeyPressed( DIK_K ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'K' );
			else
				v.push_back( 'k' );
		}
		else if (i == DIK_L && isSpecialKeyPressed( DIK_L ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'L' );
			else
				v.push_back( 'l' );
		}
		else if (i == DIK_M && isSpecialKeyPressed( DIK_M ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'M' );
			else
				v.push_back( 'm' );
		}
		else if (i == DIK_N && isSpecialKeyPressed( DIK_N ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'N' );
			else
				v.push_back( 'n' );
		}
		else if (i == DIK_O && isSpecialKeyPressed( DIK_O ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'O' );
			else
				v.push_back( 'o' );
		}
		else if (i == DIK_P && isSpecialKeyPressed( DIK_P ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'P' );
			else
				v.push_back( 'p' );
		}
		else if (i == DIK_Q && isSpecialKeyPressed( DIK_Q ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'Q' );
			else
				v.push_back( 'q' );
		}
		else if (i == DIK_R && isSpecialKeyPressed( DIK_R ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'R' );
			else
				v.push_back( 'r' );
		}
		else if (i == DIK_S && isSpecialKeyPressed( DIK_S ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'S' );
			else
				v.push_back( 's' );
		}
		else if (i == DIK_T && isSpecialKeyPressed( DIK_T ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'T' );
			else
				v.push_back( 't' );
		}
		else if (i == DIK_U && isSpecialKeyPressed( DIK_U ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'U' );
			else
				v.push_back( 'u' );
		}
		else if (i == DIK_V && isSpecialKeyPressed( DIK_V ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'V' );
			else
				v.push_back( 'v' );
		}
		else if (i == DIK_W && isSpecialKeyPressed( DIK_W ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'W' );
			else
				v.push_back( 'w' );
		}
		else if (i == DIK_X && isSpecialKeyPressed( DIK_X ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'X' );
			else
				v.push_back( 'x' );
		}
		else if (i == DIK_Y && isSpecialKeyPressed( DIK_Y ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'Y' );
			else
				v.push_back( 'y' );
		}
		else if (i == DIK_Z && isSpecialKeyPressed( DIK_Z ))
		{
			if (isKeyPressed( DIK_LSHIFT ))
				v.push_back( 'Z' );
			else
				v.push_back( 'z' );
		}
		else if (i == DIK_SPACE && isSpecialKeyPressed( DIK_SPACE ))
		{
			v.push_back( ' ' );
		}
	}
	return v;
}

CInput::~CInput()
{
	if (m_DIDevice)
	{
		m_DIDevice->Release();
		m_DIDevice = nullptr;
	}
	if (m_DIKeyboard)
	{
		m_DIKeyboard->Unacquire();
		m_DIKeyboard->Release();
		m_DIKeyboard = nullptr;
	}
	if (m_DIMouse)
	{
		m_DIMouse->Unacquire();
		m_DIMouse->Release();
		m_DIMouse = nullptr;
	}
}

bool CInput::Initialize( HINSTANCE hInstance, HWND hWnd, UINT ScreenWidth, UINT ScreenHeight )
{
	if (m_Instance)
		return true;
	try
	{
		m_Instance = new CInput( hInstance, hWnd, ScreenWidth, ScreenHeight );
	}
	catch ( ... )
	{
		return false;
	}
	return true;
}

void CInput::Destroy()
{
	if (!m_Instance)
		return;
	delete m_Instance;
	m_Instance = nullptr;
}
#pragma once
#define DIRECTINPUT_VERSION 0x0800
#define INPUT_INSTANCE CInput::GetInstance()

#include <exception>
#include <vector>
#include <algorithm>

#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")



class CInput
{
	IDirectInputDevice *m_DIKeyboard;
	IDirectInputDevice *m_DIMouse;
	IDirectInput *m_DIDevice;
	DIMOUSESTATE m_currentMouseState;
	DIMOUSESTATE m_lastMouseState;
	LONG m_ScreenWidth;
	LONG m_ScreenHeight;
	BYTE m_lastkeyboard[256];
	BYTE m_currentkeyboard[256];
	std::vector<unsigned int> m_specialkeys;
	POINT m_CursorPosition;
private:
	CInput( HINSTANCE hInstance, HWND hWnd, UINT ScreenWidth, UINT ScreenHeight );
	~CInput();
private:
	static CInput *m_Instance;
public:
	void Frame();
	std::vector<char> AlphaPressed();
public:
	static bool Initialize( HINSTANCE hInstance, HWND hWnd, UINT ScreenWidth, UINT ScreenHeight );
	static void Destroy();
	static CInput* GetInstance()
	{
		return m_Instance;
	};
public:
	inline bool isKeyPressed( UINT key )
	{
		return (m_currentkeyboard[key] & 0x80) > 0;
	};
	inline bool isSpecialKeyPressed( UINT key )
	{
		return ((m_currentkeyboard[key] & 0x80) > 0) &&
			!((m_lastkeyboard[key] & 0x80) > 0);
	};
	inline void AddSpecialKey( UINT key )
	{
		m_specialkeys.push_back( key );
	};
	inline void RemoveSpecialKey( UINT key )
	{
		m_specialkeys.erase( std::remove_if(
			m_specialkeys.begin(), m_specialkeys.end(),
			[=]( UINT expr ) -> bool
			{
				return key == expr;
			} ), m_specialkeys.end() );
	};
	inline LONG GetMouseHorizontalMove()
	{
		return m_currentMouseState.lX;
	};
	inline LONG GetMouseVerticalMove()
	{
		return m_currentMouseState.lY;
	};
	inline LONG GetCursorX()
	{
		return m_CursorPosition.x;
	};
	inline LONG GetCursorY()
	{
		return m_CursorPosition.y;
	};
};


#pragma once

#include "Graphics.h"
#include "HRTimer.h"
#include "Input.h"

class CGame
{
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	UINT m_WindowWidth;
	UINT m_WindowHeight;
	CGraphics *m_Graphics;
	CHRTimer m_HRTimer;
	bool m_bShowMenu;
	std::string m_Cheat;
public:
	CGame();
	~CGame();
public:
	bool Initialize( HINSTANCE hInstance );
	void Run();
	void Shutdown();
private:
	void HandleKeyInput();
	bool InitializeWindow();
	static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
};


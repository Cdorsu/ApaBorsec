#include "Game.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int iShow )
{
	CGame *Game = new CGame;
	if (!Game->Initialize( hInstance ))
		return 1;
	Game->Run();
	Game->Shutdown();
	delete Game;

	return 0;
}
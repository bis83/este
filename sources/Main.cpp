#include <Windows.h>

#include "Game.h"

INT APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd) {
	return Game::Run();
}

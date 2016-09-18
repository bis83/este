#pragma once
#include <Windows.h>

#include "Direct2D.h"
#include "Audio.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const LPWSTR GAME_NAME = L"este ~ the garden of funtains";
const LPWSTR FILE_NOT_FOUND = L"ファイルが開けませんでした。";

#define MESSAGE_BOX(msg) { MessageBox(NULL, msg, GAME_NAME, MB_OK); }

// システム失敗時の処理
#define FITAL_ERROR(msg) { MESSAGE_BOX(msg); PostQuitMessage(0); }
#define FITAL_FILE_ERROR(filename) { MessageBox(NULL, FILE_NOT_FOUND, filename, MB_OK); PostQuitMessage(0); }

#define SAFE_RELEASE(p) { if(p) { p->Release(); p = 0; } }

class Game {
	Game(HWND hWnd);
	~Game();

	Drawer *pDraw;
	Audio *pAudio;

	DWORD prev, now, advance, frame, frame_time;

	void Update();

	static Game *m_pInstance;
	static bool m_bWindowed;
	static HWND hWnd;

	static LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static void SetFocus();
	static void KillFocus();
public:

	// ゲームの起動
	static int Run();

	// 描画処理と音響処理の取得
	static Drawer *GetDrawer();
	static Audio *GetAudio();
	static HWND GetHWND() { return hWnd; }
};

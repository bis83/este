#include "Game.h"

#include "Board.h"
#include "Sprite.h"
#include "Sound.h"

// グローバルな変数
board g_board;

Game *Game::m_pInstance = NULL;
bool Game::m_bWindowed = true;
HWND Game::hWnd = NULL;

LRESULT WINAPI Game::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	switch(Msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_SETFOCUS:
		SetFocus();
		break;
	case WM_KILLFOCUS:
		KillFocus();
		break;
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		break;
	case WM_LBUTTONDOWN:	// 左クリック処理
		g_board.click();
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

int Game::Run() {
	// COMのマルチスレッド初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// ウィンドウクラスの作成
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = NULL;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = GAME_NAME;

	// ウィンドウクラスの登録
	if( 0 == RegisterClassEx(&wcex)) {
		MESSAGE_BOX(L"WindowClassの登録に失敗");
		return 0;
	}

	// モード設定
	if(MessageBox(NULL, L"フルスクリーンモードで起動しますか？", GAME_NAME, MB_YESNO) == 6) {
		m_bWindowed = false;
		// ウィンドウの生成
		if( NULL == (hWnd = CreateWindow(
			GAME_NAME,
			GAME_NAME,
			WS_POPUP,
			0,
			0,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			NULL,
			NULL,
			NULL,
			NULL)))
		{
			MESSAGE_BOX(L"Windowの生成に失敗");
			return 0;
		}
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		// フルスクリーンへの切り替え
		DEVMODE dev;
		dev.dmSize       = sizeof(DEVMODE);
		dev.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
		dev.dmPelsWidth  = SCREEN_WIDTH;
		dev.dmPelsHeight = SCREEN_HEIGHT;
		ChangeDisplaySettings(&dev, CDS_FULLSCREEN);
	} else {
		m_bWindowed = true;
		// ウィンドウの生成
		if( NULL == (hWnd = CreateWindow(
			GAME_NAME,
			GAME_NAME,
			WS_CAPTION | WS_SYSMENU,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			SCREEN_WIDTH + GetSystemMetrics(SM_CXFIXEDFRAME),
			SCREEN_HEIGHT + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME),
			NULL,
			NULL,
			NULL,
			NULL)))
		{
			MESSAGE_BOX(L"Windowの生成に失敗");
			return 0;
		}
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}

	// ゲームコンポーネントクラスの初期化
	Game *pGame = new Game(hWnd);
	InitSprite();
	sb.init();

	g_board.init();

	// メインループ
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			// メッセージの翻訳、送信
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// メイン処理
			pGame->Update();
		}
	}

	if(!m_bWindowed) {
		ChangeDisplaySettings(NULL, 0);
	}

	// 終了処理
	sb.release();
	ReleaseSprite();
	delete pGame;

	return 0;
};

Game::Game(HWND hWnd) :
now(timeGetTime()), prev(timeGetTime()), frame_time(1000/60), frame(0)
{
	// インスタンスの登録
	m_pInstance = this;

	// ゲームで使用するリソースを生成
	pDraw = new Drawer(hWnd);
	pAudio = new Audio();
}

Game::~Game() {
	// ゲームで使用するリソースの解放
	delete pDraw;
	delete pAudio;
}

void Game::Update() {
	// sleep処理
	now = timeGetTime();
	if(now-prev < frame_time * frame) {
		Sleep(frame_time*frame-(now-prev));
	}
	if(now - prev >= 1000) {
		prev = now;
		frame = 0;
	}
	frame++;

	// ゲーム処理
	g_board.update();

	// 描画処理
	pDraw->BeginRender();
	pDraw->ResetTranslation();
	pDraw->Clear(0.0f, 0.0f, 0.0f);

	g_board.draw();
	
	pDraw->EndRender();
}

void Game::SetFocus() {
	if(!m_bWindowed) {
		DEVMODE dev;
		dev.dmSize = sizeof(DEVMODE);
		dev.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		dev.dmPelsWidth = SCREEN_WIDTH;
		dev.dmPelsHeight = SCREEN_HEIGHT;
		ChangeDisplaySettings(&dev, CDS_FULLSCREEN);
	}
}

void Game::KillFocus() {
	if(!m_bWindowed) {
		ChangeDisplaySettings(NULL, 0);
		CloseWindow(hWnd);
	}
}

Drawer *Game::GetDrawer() {
	return m_pInstance->pDraw;
}

Audio *Game::GetAudio() {
	return m_pInstance->pAudio;
}

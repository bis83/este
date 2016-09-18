#include "Game.h"

#include "Board.h"
#include "Sprite.h"
#include "Sound.h"

// �O���[�o���ȕϐ�
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
	case WM_LBUTTONDOWN:	// ���N���b�N����
		g_board.click();
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

int Game::Run() {
	// COM�̃}���`�X���b�h������
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// �E�B���h�E�N���X�̍쐬
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

	// �E�B���h�E�N���X�̓o�^
	if( 0 == RegisterClassEx(&wcex)) {
		MESSAGE_BOX(L"WindowClass�̓o�^�Ɏ��s");
		return 0;
	}

	// ���[�h�ݒ�
	if(MessageBox(NULL, L"�t���X�N���[�����[�h�ŋN�����܂����H", GAME_NAME, MB_YESNO) == 6) {
		m_bWindowed = false;
		// �E�B���h�E�̐���
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
			MESSAGE_BOX(L"Window�̐����Ɏ��s");
			return 0;
		}
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		// �t���X�N���[���ւ̐؂�ւ�
		DEVMODE dev;
		dev.dmSize       = sizeof(DEVMODE);
		dev.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
		dev.dmPelsWidth  = SCREEN_WIDTH;
		dev.dmPelsHeight = SCREEN_HEIGHT;
		ChangeDisplaySettings(&dev, CDS_FULLSCREEN);
	} else {
		m_bWindowed = true;
		// �E�B���h�E�̐���
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
			MESSAGE_BOX(L"Window�̐����Ɏ��s");
			return 0;
		}
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}

	// �Q�[���R���|�[�l���g�N���X�̏�����
	Game *pGame = new Game(hWnd);
	InitSprite();
	sb.init();

	g_board.init();

	// ���C�����[�v
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			// ���b�Z�[�W�̖|��A���M
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// ���C������
			pGame->Update();
		}
	}

	if(!m_bWindowed) {
		ChangeDisplaySettings(NULL, 0);
	}

	// �I������
	sb.release();
	ReleaseSprite();
	delete pGame;

	return 0;
};

Game::Game(HWND hWnd) :
now(timeGetTime()), prev(timeGetTime()), frame_time(1000/60), frame(0)
{
	// �C���X�^���X�̓o�^
	m_pInstance = this;

	// �Q�[���Ŏg�p���郊�\�[�X�𐶐�
	pDraw = new Drawer(hWnd);
	pAudio = new Audio();
}

Game::~Game() {
	// �Q�[���Ŏg�p���郊�\�[�X�̉��
	delete pDraw;
	delete pAudio;
}

void Game::Update() {
	// sleep����
	now = timeGetTime();
	if(now-prev < frame_time * frame) {
		Sleep(frame_time*frame-(now-prev));
	}
	if(now - prev >= 1000) {
		prev = now;
		frame = 0;
	}
	frame++;

	// �Q�[������
	g_board.update();

	// �`�揈��
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

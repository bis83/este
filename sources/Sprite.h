#pragma once

#include "Direct2D.h"
#include "Board.h"

// �����X�v���C�g�̕`��
struct fountain_sprite {
	static Bitmap *pBitmap;
	float x, y;
	float width, height;
	int time;

	// �`��(px, py:��ʈʒu, water_f:�����ʂ��Ă��邩)
	void draw(float px, float py, bool water_f);
	void drawnext(float px, float py);
	void drawcursor(float px, float py);
};

// �{�[�h�`��̃X�v���C�g
struct board_sprite {
	static Bitmap *pBitmap;

	void draw(float px, float py);
};

// �����`��X�v���C�g
struct fountainhead_sprite {
	static Bitmap *pBitmap;
	static TextFormat *pTLayout;
	static Brush *pTBrush;

	void draw(float px, float py, int water, int time, joint_dir dir);
};

// �w�i�`��X�v���C�g
struct background_sprite {
	static Bitmap *pBitmap;

	void draw();
};

// �X�R�A�`��X�v���C�g
struct score_sprite {
	static TextFormat *pTLayout;
	static Brush *pTBrush;

	void draw(float x, float y, int score);
};

// �X�^�[�g�`��X�v���C�g
struct prestart_sprite {
	static Bitmap *pBitmap;

	void draw(int time);
};

struct gameover_sprite {
	static Bitmap *pBitmap;
	static TextFormat *pTLayout;
	static Brush *pTBrush;

	void draw(int time, int score);
};

extern fountain_sprite s_fountains[10];
extern board_sprite s_board;
extern fountainhead_sprite s_fountainhead;
extern background_sprite s_background;
extern score_sprite s_score;
extern prestart_sprite s_prestart;
extern gameover_sprite s_gameover;

// �X�v���C�g�̏�����
bool InitSprite();
// �X�v���C�g�̊J��
void ReleaseSprite();

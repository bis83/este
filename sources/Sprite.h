#pragma once

#include "Direct2D.h"
#include "Board.h"

// 噴水スプライトの描画
struct fountain_sprite {
	static Bitmap *pBitmap;
	float x, y;
	float width, height;
	int time;

	// 描画(px, py:画面位置, water_f:水が通っているか)
	void draw(float px, float py, bool water_f);
	void drawnext(float px, float py);
	void drawcursor(float px, float py);
};

// ボード描画のスプライト
struct board_sprite {
	static Bitmap *pBitmap;

	void draw(float px, float py);
};

// 水源描画スプライト
struct fountainhead_sprite {
	static Bitmap *pBitmap;
	static TextFormat *pTLayout;
	static Brush *pTBrush;

	void draw(float px, float py, int water, int time, joint_dir dir);
};

// 背景描画スプライト
struct background_sprite {
	static Bitmap *pBitmap;

	void draw();
};

// スコア描画スプライト
struct score_sprite {
	static TextFormat *pTLayout;
	static Brush *pTBrush;

	void draw(float x, float y, int score);
};

// スタート描画スプライト
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

// スプライトの初期化
bool InitSprite();
// スプライトの開放
void ReleaseSprite();

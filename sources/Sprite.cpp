#include "Sprite.h"
#include "Game.h"

#include <sstream>
using namespace std;

Bitmap *fountain_sprite::pBitmap = NULL;
Bitmap *board_sprite::pBitmap = NULL;
Bitmap *fountainhead_sprite::pBitmap = NULL;
TextFormat *fountainhead_sprite::pTLayout = NULL;
Brush *fountainhead_sprite::pTBrush = NULL;
Bitmap *background_sprite::pBitmap = NULL;
TextFormat *score_sprite::pTLayout = NULL;
Brush *score_sprite::pTBrush = NULL;
Bitmap *prestart_sprite::pBitmap = NULL;

Bitmap *gameover_sprite::pBitmap = NULL;
TextFormat *gameover_sprite::pTLayout = NULL;
Brush *gameover_sprite::pTBrush = NULL;

fountain_sprite s_fountains[10];
board_sprite s_board;
fountainhead_sprite s_fountainhead;
background_sprite s_background;
score_sprite s_score;
prestart_sprite s_prestart;
gameover_sprite s_gameover;

bool InitSprite() {
	s_background.pBitmap = new Bitmap(Game::GetDrawer(), L"rsc/background.png");
	s_board.pBitmap = new Bitmap(Game::GetDrawer(), L"rsc/board.png");

	fountain_sprite::pBitmap = new Bitmap(Game::GetDrawer(), L"rsc/fountains.png");

	fountainhead_sprite::pBitmap = new Bitmap(Game::GetDrawer(), L"rsc/fountainhead.png");
	fountainhead_sprite::pTLayout = new TextFormat(Game::GetDrawer(), L"Times New Roman", 800, 0, 5, 48.0f);
	fountainhead_sprite::pTBrush = new Brush(Game::GetDrawer(), 0.0f, 0.0f, 0.2f);
	fountainhead_sprite::pTLayout->SetBrush(fountainhead_sprite::pTBrush);

	score_sprite::pTLayout = new TextFormat(Game::GetDrawer(), L"Times New Roman", 800, 0, 5, 36.0f);
	score_sprite::pTBrush = new Brush(Game::GetDrawer(), 0.2f, 0.2f, 0.0f);
	score_sprite::pTLayout->SetBrush(score_sprite::pTBrush);

	prestart_sprite::pBitmap = new Bitmap(Game::GetDrawer(), L"rsc/prestart.png");
	gameover_sprite::pBitmap = new Bitmap(Game::GetDrawer(), L"rsc/gameover.png");
	gameover_sprite::pTLayout = new TextFormat(Game::GetDrawer(), L"Times New Roman", 600, 0, 5, 48.0f);
	gameover_sprite::pTBrush = new Brush(Game::GetDrawer(), 0.0f, 0.0f, 0.0f);
	gameover_sprite::pTLayout->SetBrush(gameover_sprite::pTBrush);
	
	s_fountains[WATER_LOAD].x = 0;
	s_fountains[WATER_LOAD].y = 0;
	s_fountains[WATER_LOAD].width = 200;
	s_fountains[WATER_LOAD].height = 100;

	s_fountains[FRONT_OF_HOUSE].x = 0;
	s_fountains[FRONT_OF_HOUSE].y = 100;
	s_fountains[FRONT_OF_HOUSE].width = 100;
	s_fountains[FRONT_OF_HOUSE].height = 200;

	s_fountains[NEPTUNE].x = 400;
	s_fountains[NEPTUNE].y = 100;
	s_fountains[NEPTUNE].width = 200;
	s_fountains[NEPTUNE].height = 200;

	s_fountains[CIRCUIT_TOP_LEFT].x = 400;
	s_fountains[CIRCUIT_TOP_LEFT].y = 500;
	s_fountains[CIRCUIT_TOP_LEFT].width = 100;
	s_fountains[CIRCUIT_TOP_LEFT].height = 100;

	s_fountains[CIRCUIT_TOP_RIGHT].x = 400;
	s_fountains[CIRCUIT_TOP_RIGHT].y = 300;
	s_fountains[CIRCUIT_TOP_RIGHT].width = 100;
	s_fountains[CIRCUIT_TOP_RIGHT].height = 100;

	s_fountains[CIRCUIT_RIGHT_DOWN].x = 400;
	s_fountains[CIRCUIT_RIGHT_DOWN].y = 400;
	s_fountains[CIRCUIT_RIGHT_DOWN].width = 100;
	s_fountains[CIRCUIT_RIGHT_DOWN].height = 100;

	s_fountains[CIRCUIT_DOWN_LEFT].x = 0;
	s_fountains[CIRCUIT_DOWN_LEFT].y = 500;
	s_fountains[CIRCUIT_DOWN_LEFT].width = 100;
	s_fountains[CIRCUIT_DOWN_LEFT].height = 100;

	s_fountains[VIRNUS].x = 0;
	s_fountains[VIRNUS].y = 600;
	s_fountains[VIRNUS].width = 200;
	s_fountains[VIRNUS].height = 100;

	s_fountains[DRAGON_RIGHT].x = 0;
	s_fountains[DRAGON_RIGHT].y = 700;
	s_fountains[DRAGON_RIGHT].width = 100;
	s_fountains[DRAGON_RIGHT].height = 200;

	s_fountains[DRAGON_LEFT].x = 400;
	s_fountains[DRAGON_LEFT].y = 700;
	s_fountains[DRAGON_LEFT].width = 100;
	s_fountains[DRAGON_LEFT].height = 200;
	
	return false;
}

void ReleaseSprite() {
	delete fountain_sprite::pBitmap;
	delete board_sprite::pBitmap;
	delete fountainhead_sprite::pBitmap;
	delete fountainhead_sprite::pTBrush;
	delete fountainhead_sprite::pTLayout;
	delete background_sprite::pBitmap;
	delete score_sprite::pTBrush;
	delete score_sprite::pTLayout;
	delete prestart_sprite::pBitmap;
	delete gameover_sprite::pBitmap;
	delete gameover_sprite::pTBrush;
	delete gameover_sprite::pTLayout;
}


void fountain_sprite::draw(float px, float py, bool water_f) {
	Translation trans(Game::GetDrawer());
	trans.AddPos(px, py);
	trans.Bind();

	++time;
	if(time > 120) {
		time = 0;
	}
	float x = this->x + (!water_f ? width * 2 : 0) + (time > 60 ? width : 0);
	float y = this->y;
	if(x >= 800.0f) {
		x -= 800.0f;
		y += height;
	}
	pBitmap->Draw(x, x + width, y, y + height);
}

void fountain_sprite::drawnext(float px, float py) {
	Translation trans(Game::GetDrawer());
	trans.AddScale(0.0f, 0.0f, 0.5f, 0.5f);
	trans.AddPos(px, py);
	trans.Bind();

	pBitmap->Draw(x, x+width, y, y+height);
}

void fountain_sprite::drawcursor(float px, float py) {
	Translation trans(Game::GetDrawer());
	trans.AddPos(px, py);
	trans.Bind();

	pBitmap->Draw(0.5f, x, x+width, y, y+height);
}

void board_sprite::draw(float px, float py) {
	Translation trans(Game::GetDrawer());
	trans.AddPos(px, py);
	trans.Bind();
	pBitmap->Draw();
}

void fountainhead_sprite::draw(float px, float py, int water, int time, joint_dir dir) {
	Translation trans(Game::GetDrawer());
	trans.AddPos(px, py);
	trans.Bind();

	float left;
	switch(dir) {
	case DOWN:
		left = 0;
		break;
	case LEFT:
		left = 100;
		break;
	case UP:
		left = 200;
		break;
	case RIGHT:
		left = 300;
		break;
	}
	float right = left + 100.0f;
	float top = (water > 0 ? 0 : 100);
	float bottom = top + 100;
	pBitmap->Draw(left, right, top, bottom);

	trans.Reset();
	trans.AddPos(px+26, py+26);
	trans.Bind();

	wstringstream ss;
	ss << water;
	pTLayout->Draw(ss.str());
}

void background_sprite::draw() {
	Game::GetDrawer()->ResetTranslation();
	pBitmap->Draw();
}

void score_sprite::draw(float x, float y, int score) {
	Translation trans(Game::GetDrawer());
	trans.AddPos(x, y);
	trans.Bind();

	wstringstream ss;
	ss << L"SCORE : ";
	ss << score;
	pTLayout->Draw(ss.str());
}

void prestart_sprite::draw(int time) {
	Game::GetDrawer()->ResetTranslation();
	pBitmap->Draw(1.0f * (30 - time) / 30);
}

void gameover_sprite::draw(int time, int score) {
	Game::GetDrawer()->ResetTranslation();
	pBitmap->Draw(1.0f * (30 - time) / 30);

	Translation trans(Game::GetDrawer());
	trans.AddPos(320, 320);
	trans.Bind();

	wstringstream ss;
	ss << L"score : ";
	ss << score;
	pTLayout->Draw(ss.str());
}

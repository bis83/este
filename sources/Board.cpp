#include "Board.h"
#include "Sprite.h"
#include "Game.h"
#include "Sound.h"

void fountain::set(int no) {
	type = (fountain_type)no;
	x = y = 0;
	water_f = false;
	switch(no) {
	case WATER_LOAD:
		width = 2;
		height = 1;
		score = 6;
		break;
	case FRONT_OF_HOUSE:
		width = 1;
		height = 2;
		score = 6;
		break;
	case NEPTUNE:
		width = 2;
		height = 2;
		score = 5;
		break;
	case CIRCUIT_TOP_LEFT:
		width = 1;
		height = 1;
		score = 2;
		break;
	case CIRCUIT_TOP_RIGHT:
		width = 1;
		height = 1;
		score = 2;
		break;
	case CIRCUIT_RIGHT_DOWN:
		width = 1;
		height = 1;
		score = 2;
		break;
	case CIRCUIT_DOWN_LEFT:
		width = 1;
		height = 1;
		score = 2;
		break;
	case DRAGON_LEFT:
		width = 1;
		height = 2;
		score = 3;
		break;
	case DRAGON_RIGHT:
		width = 1;
		height = 2;
		score = 3;
		break;
	case VIRNUS:
		width = 2;
		height = 1;
		score = 3;
		break;
	}
}

void fountain::set(fountain &next, uint8 x, uint8 y) {
	this->x = x;
	this->y = y;
	this->width = next.width;
	this->height = next.height;
	this->type = next.type;
	this->water_f = false;
	this->score = next.score;
}

void board::update() {
	if(time > 0) {
		--time;
		return;
	}

	if(!prestart_f) {
		return;
	}

	if(gameover_f) {
		return;
	}

	// 水源の時間経過
	for(uint8 i=0; i<4; ++i) {
		if(d_fountainhead[i].water) {
			// 水の自然減少
			--d_fountainhead[i].time;
			if(d_fountainhead[i].water == 0) {
				sb.pSoundBank->Play(sb.s_id[0]);
			}
			if(d_fountainhead[i].time <= 0) {
				--d_fountainhead[i].water;
				d_fountainhead[i].time = 60;
			}
		} else {
			// ともかくタイムは60になる
			d_fountainhead[i].time = 60;
		}
	}

	for(uint8 i=0; i<4; ++i) {
		if(d_fountainhead[i].water > 0) {
			return;
		}
	}

	// ゲームオーバー
	gameover_f = true;
	time = 30;
	sb.pSoundBank->Play(sb.s_id[4]);
	sb.pMusic->Stop();
}

void board::init() {
	score = 0;
	init_f = true;
	
	srand(timeGetTime());
	for(uint8 i=0; i<4; ++i) {
		d_next[i].set(rand()%10);
	}

	for(uint8 i=0; i<4; ++i) {
		d_fountainhead[i].time = 60;
		d_fountainhead[i].water = 50+i*10;
	}

	reset();

	prestart_f = false;
	time = 30;
	gameover_f = false;

	sb.pSoundBank->Play(sb.s_id[3]);
}

void board::click() {
	if(!init_f) {
		return;
	}

	if(time > 0) {
		return;
	}

	if(!prestart_f) {
		// ゲームの開始
		sb.pMusic->Play();
		prestart_f = true;
		return;
	}

	if(gameover_f) {
		init();
		return;
	}

	// マウス位置の取得
	POINT mp;
	GetCursorPos(&mp);
	ScreenToClient(Game::GetHWND(), &mp);

	// 噴水の設置
	if(mp.x > 100 && mp.y > 100 && mp.x < 500 && mp.y < 500) {
		uint8 bx = mp.x / 100 - 1;
		uint8 by = mp.y / 100 - 1;
		// ボードとの判定
		if(enablePut(bx, by, d_next[0])) {
			// 設置
			sb.pSoundBank->Play(sb.s_id[2]);
			addFountain(bx, by);
		}
	}

	// 各水源との判定
	if(mp.x > 410 && mp.y > 10 && mp.x < 490 && mp.y < 90) {
		sb.pSoundBank->Play(sb.s_id[0]);
		d_fountainhead[0].water = 0;
		d_fountainhead[0].time = 60;
	}
	if(mp.x > 510 && mp.y > 410 && mp.x < 590 && mp.y < 490) {
		sb.pSoundBank->Play(sb.s_id[0]);
		d_fountainhead[1].water = 0;
		d_fountainhead[1].time = 60;
	}
	if(mp.x > 110 && mp.y > 510 && mp.x < 190 && mp.y < 590) {
		sb.pSoundBank->Play(sb.s_id[0]);
		d_fountainhead[2].water = 0;
		d_fountainhead[2].time = 60;
	}
	if(mp.x > 10 && mp.y > 110 && mp.x < 90 && mp.y < 190) {
		sb.pSoundBank->Play(sb.s_id[0]);
		d_fountainhead[3].water = 0;
		d_fountainhead[3].time = 60;
	}

	// 水路の判定
	bool check[4][4];
	board_elem *elem_list[16];

	for(uint8 i=0; i<4; ++i)
		for(uint8 j=0; j<4; ++j)
			check[i][j] = false;
	for(uint8 i=0; i<16; ++i) {
		elem_list[i] = 0;
	}

	// 接続判定
	for(uint8 i=0; i<3; ++i) {
		for(uint8 j=i+1; j<4; ++j) {
			// iとjが接続されているか判定する
			if((d_fountainhead[i].water > 0 || d_fountainhead[j].water > 0)
				&& joined(i, j, check, elem_list))
			{
				int water = 0, line = 0, score = 1;
				for(uint8 k=0; k<16; ++k) {

					// ボードの要素と噴水を初期化する
					if(elem_list[k]) {

						// ライン数を数える
						++line;

						// まだ精算していない噴水に対して
						if(elem_list[k]->pFount->type != NOTHING) {

							// 水の追加
							if(elem_list[k]->pFount->type == WATER_LOAD) {
								water += 6;
							} else if(elem_list[k]->pFount->type == NEPTUNE) {
								water += 4;
							} else if(elem_list[k]->pFount->type == FRONT_OF_HOUSE) {
								water += 6;
							} else if(elem_list[k]->pFount->type == DRAGON_LEFT) {
								water += 3;
							} else if(elem_list[k]->pFount->type == DRAGON_RIGHT) {
								water += 3;
							} else if(elem_list[k]->pFount->type == VIRNUS) {
								water += 3;
							}

							// スコアの獲得
							score *= elem_list[k]->pFount->score;
						}

						// 要素の消去
						elem_list[k]->pFount->type = NOTHING;
					}
				}
				// 総合スコアの獲得
				sb.pSoundBank->Play(sb.s_id[1]);
				this->score += score;
				// 水源への水の追加
				if(d_fountainhead[i].water < d_fountainhead[j].water) {
					d_fountainhead[i].water += water * line;
					if(d_fountainhead[i].water > 100)
						d_fountainhead[i].water = 100;
					d_fountainhead[i].time = 60;
				} else {
					d_fountainhead[j].water += water * line;
					if(d_fountainhead[j].water > 100)
						d_fountainhead[j].water = 100;
					d_fountainhead[j].time = 60;
				}
			}
			// チェックと要素リストのリセット
			for(uint8 i=0; i<4; ++i) for(uint8 j=0; j<4; ++j) check[i][j] = false;
			for(uint8 i=0; i<16; ++i) elem_list[i] = 0;
		}
	}

	// 無効な要素をボードから取り除く
	for(uint8 i=0; i<4; ++i) {
		for(uint8 j=0; j<4; ++j) {
			if(elems[i][j].pFount) {
				if(elems[i][j].pFount->type == NOTHING) {
					elems[i][j].pFount = 0;
					elems[i][j].joint[UP] =
							elems[i][j].joint[RIGHT] =
							elems[i][j].joint[DOWN] =
							elems[i][j].joint[LEFT] = false;
				}
			}
		}
	}

	// ボードに流れる水を判定する
	for(uint8 i=0; i<16; ++i) d_fountain[i].water_f = false;
	for(uint8 i=0; i<4; ++i) for(uint8 j=0; j<4; ++j) check[i][j] = false;
	for(uint8 i=0; i<16; ++i) elem_list[i] = 0;
	for(uint8 i=0; i<4; ++i) {
		for(uint8 j=0; j<4; ++j) {
			if(i == j) continue;
			// iとjが接続されているか判定する
			if(d_fountainhead[i].water > 0) {
				joined(i, j, check, elem_list);
				for(uint8 k=0; k<4; ++k) {
					for(uint8 l=0; l<4; ++l) {
						if(check[k][l]) {
							elems[k][l].pFount->water_f = true;
						}
					}
				}
			}
			for(uint8 i=0; i<4; ++i) for(uint8 j=0; j<4; ++j) check[i][j] = false;
			for(uint8 i=0; i<16; ++i) elem_list[i] = 0;
		}
	}

	// すべてのボードが水に満ちている場合は得点
	for(uint8 i=0; i<4; ++i) {
		bool end_f = false;
		for(uint8 j=0; j<4; ++j) {
			if(!elems[i][j].pFount || (elems[i][j].pFount && !elems[i][j].pFount->water_f)) {
				end_f = true;
				break;
			}
			if(i == j == 3) {
				// パーフェクト
				sb.pSoundBank->Play(sb.s_id[1]);
				score += 10000;
				for(uint8 k=0; k<4; ++k) {
					d_fountainhead[k].water += 25;
					if(d_fountainhead[k].water >= 100) {
						d_fountainhead[k].water = 100;
					}
					d_fountainhead[k].time = 60;
				}
			}
		}
		if(end_f) {
			break;
		}
	}

	// ボードが満タンになっているか調べる
	for(uint8 i=0; i <= 4 - d_next[0].width; ++i) {
		for(uint8 j=0; j <= 4 - d_next[0].height; ++j) {
			if(elems[i][j].pFount == NULL &&
				(d_next[0].width >= 2 ? (elems[i+1][j].pFount == NULL) : 1) &&
				(d_next[0].height >= 2 ? (elems[i][j+1].pFount == NULL) : 1) &&
				((d_next[0].width >= 2 && d_next[0].height >= 2) ? (elems[i+1][j+1].pFount == NULL) : 1)
				)
			{
				return;
			}
		}
	}
	sb.pSoundBank->Play(sb.s_id[5]);
	reset();
}

bool board::enablePut(uint8 x, uint8 y, fountain &next) {
	// ボードに収まりきらない位置の場合
	if((x+next.width > 4) || (y+next.height > 4))
		return false;

	// すでにボードに配置されている噴水と競合する場合
	for(uint8 i=x; i < x + next.width; ++i) {
		for(uint8 j=y; j < y + next.height; ++j) {
			if(elems[i][j].pFount) {
				return false;
			}
		}
	}
	
	// 満たない場合は設置可能
	return true;
}

void board::draw() {
	// 背景の描画
	s_background.draw();

	// ボードの描画
	s_board.draw(50, 50);

	// 水源の描画
	s_fountainhead.draw(400, 0, d_fountainhead[0].water, d_fountainhead[0].time, DOWN);
	s_fountainhead.draw(500, 400, d_fountainhead[1].water, d_fountainhead[1].time, LEFT);
	s_fountainhead.draw(100, 500, d_fountainhead[2].water, d_fountainhead[2].time, UP);
	s_fountainhead.draw(0, 100, d_fountainhead[3].water, d_fountainhead[3].time, RIGHT);

	// 噴水の描画
	for(uint8 i=0; i<16; ++i) {
		if(d_fountain[i].type != NOTHING) {
			s_fountains[d_fountain[i].type].draw(d_fountain[i].x*100+100, d_fountain[i].y*100+100, d_fountain[i].water_f);
		}
	}

	// 配置可能な噴水の描画
	if(prestart_f && !gameover_f) {
		POINT mp;
		GetCursorPos(&mp);
		ScreenToClient(Game::GetHWND(), &mp);

		if(mp.x > 100 && mp.y > 100 && mp.x < 500 && mp.y < 500) {
			if(enablePut(mp.x/100 - 1, mp.y/100 - 1, d_next[0])) {
				// 描画
				s_fountains[d_next[0].type].drawcursor((mp.x / 100) * 100, (mp.y / 100) * 100);
			}
		}
	}

	// ネクストの描画
	for(uint8 i=0; i<4; ++i) {
		if(d_next[i].type != NOTHING) {
			s_fountains[d_next[i].type].drawnext(640, 60 + i*120);
		}
	}

	// スコアの描画
	s_score.draw(32, 8, score);

	// プリスタートの描画
	if(!prestart_f) {
		s_prestart.draw(time);
	}

	// ゲームオーバーの描画
	if(gameover_f) {
		s_gameover.draw(time, score);
	}
}

bool board::joined_func(int goal, uint8 x, uint8 y, joint_dir src_dir, bool check[4][4], board_elem *elemlist[16]) {
	// 前の地点との判定
	if(!elems[x][y].joint[src_dir]) {
		return false;
	}

	// チェックを付ける
	check[x][y] = true;

	// ゴールの判定
	if(goal == 0 && x == 3 && y == 0 && elems[x][y].joint[UP] ||
		goal == 1 && x == 3 && y == 3 && elems[x][y].joint[RIGHT] ||
		goal == 2 && x == 0 && y == 3 && elems[x][y].joint[DOWN] ||
		goal == 3 && x == 0 && y == 0 && elems[x][y].joint[LEFT])
	{
		// 成功
		// リストにエレメントを追加
		for(uint8 i=0; i<16; ++i) {
			if(!elemlist[i]) {
				elemlist[i] = &elems[x][y];
				break;
			}
		}
		return true;
	}

	// 次の地点への再帰
	if(elems[x][y].joint[UP] && src_dir != UP) {
		if(y-1 >= 0 && !check[x][y-1] && joined_func(goal, x, y-1, DOWN, check, elemlist)) {
			// リストに追加していく
			for(uint8 i=0; i<16; ++i) {
				if(!elemlist[i]) {
					elemlist[i] = &elems[x][y];
					break;
				}
			}
			return true;
		}
	}
	if(elems[x][y].joint[RIGHT] && src_dir != RIGHT) {
		if(x+1 <= 3 && !check[x+1][y] && joined_func(goal, x+1, y, LEFT, check, elemlist)) {
			for(uint8 i=0; i<16; ++i) {
				if(!elemlist[i]) {
					elemlist[i] = &elems[x][y];
					break;
				}
			}
			return true;
		}
	}
	if(elems[x][y].joint[DOWN] && src_dir != DOWN) {
		if(y+1 <= 3 && !check[x][y+1] && joined_func(goal, x, y+1, UP, check, elemlist)) {
			for(uint8 i=0; i<16; ++i) {
				if(!elemlist[i]) {
					elemlist[i] = &elems[x][y];
					break;
				}
			}
			return true;
		}
	}
	if(elems[x][y].joint[LEFT] && src_dir != LEFT) {
		if(x-1 >= 0  && !check[x-1][y] && joined_func(goal, x-1, y, RIGHT, check, elemlist)) {
			for(uint8 i=0; i<16; ++i) {
				if(!elemlist[i]) {
					elemlist[i] = &elems[x][y];
					break;
				}
			}
			return true;
		}
	}

	return false;
}

bool board::joined(int i, int j, bool check[4][4], board_elem *elemlist[16]) {
	if(i == 0) {
		return joined_func(j, 3, 0, UP, check, elemlist);
	} else if(i == 1) {
		return joined_func(j, 3, 3, RIGHT, check, elemlist);
	} else if(i == 2) {
		return joined_func(j, 0, 3, DOWN, check, elemlist);
	} else if(i == 3) {
		return joined_func(j, 0, 0, LEFT, check, elemlist);
	}
}

void board::addFountain(uint8 x, uint8 y) {
	fountain *p = 0;
	for(uint8 i=0; i<16; ++i) {
		if(d_fountain[i].type == NOTHING) {
			d_fountain[i].set(d_next[0], x, y);
			p = &d_fountain[i];
			break;
		}
	}

	for(uint8 i=0; i<3; ++i) {
		d_next[i].set(d_next[i+1], 0, 0);
	}
	d_next[3].set(rand()%10);

	switch(p->type) {
	case WATER_LOAD:
		elems[x][y].pFount = p;
		elems[x+1][y].pFount = p;
		elems[x][y].joint[LEFT] = elems[x][y].joint[RIGHT] = true;
		elems[x][y].joint[UP] = elems[x][y].joint[DOWN] = false;
		elems[x+1][y].joint[LEFT] = elems[x+1][y].joint[RIGHT] = true;
		elems[x+1][y].joint[UP] = elems[x+1][y].joint[DOWN] = false;
		break;
	case FRONT_OF_HOUSE:
		elems[x][y].pFount = p;
		elems[x][y+1].pFount = p;
		elems[x][y].joint[LEFT] = elems[x][y].joint[RIGHT] = false;
		elems[x][y].joint[UP] = elems[x][y].joint[DOWN] = true;
		elems[x][y+1].joint[LEFT] = elems[x][y+1].joint[RIGHT] = false;
		elems[x][y+1].joint[UP] = elems[x][y+1].joint[DOWN] = true;
		break;
	case NEPTUNE:
		elems[x][y].pFount = p;
		elems[x+1][y].pFount = p;
		elems[x][y+1].pFount = p;
		elems[x+1][y+1].pFount = p;
		elems[x][y].joint[LEFT] = elems[x][y].joint[RIGHT] = elems[x][y].joint[DOWN] = true;
		elems[x][y].joint[UP] = false;
		elems[x+1][y].joint[LEFT] = elems[x+1][y].joint[UP] = elems[x+1][y].joint[DOWN] = true;
		elems[x+1][y].joint[RIGHT] = false;
		elems[x][y+1].joint[UP] = elems[x][y+1].joint[RIGHT] = elems[x][y+1].joint[DOWN] = true;
		elems[x][y+1].joint[LEFT] = false;
		elems[x+1][y+1].joint[LEFT] = elems[x+1][y+1].joint[RIGHT] = elems[x+1][y+1].joint[UP] = true;
		elems[x+1][y+1].joint[DOWN] = false;
		break;
	case CIRCUIT_TOP_LEFT:
		elems[x][y].pFount = p;
		elems[x][y].joint[LEFT] = elems[x][y].joint[UP] = true;
		elems[x][y].joint[RIGHT] = elems[x][y].joint[DOWN] = false;
		break;
	case CIRCUIT_TOP_RIGHT:
		elems[x][y].pFount = p;
		elems[x][y].joint[RIGHT] = elems[x][y].joint[UP] = true;
		elems[x][y].joint[LEFT] = elems[x][y].joint[DOWN] = false;
		break;
	case CIRCUIT_RIGHT_DOWN:
		elems[x][y].pFount = p;
		elems[x][y].joint[LEFT] = elems[x][y].joint[UP] = false;
		elems[x][y].joint[RIGHT] = elems[x][y].joint[DOWN] = true;
		break;
	case CIRCUIT_DOWN_LEFT:
		elems[x][y].pFount = p;
		elems[x][y].joint[LEFT] = elems[x][y].joint[DOWN] = true;
		elems[x][y].joint[RIGHT] = elems[x][y].joint[UP] = false;
		break;
	case DRAGON_LEFT:
		elems[x][y].pFount = p;
		elems[x][y+1].pFount = p;
		elems[x][y].joint[RIGHT] = false;
		elems[x][y].joint[UP] = elems[x][y].joint[LEFT] = elems[x][y].joint[DOWN] = true;
		elems[x][y+1].joint[LEFT] = false;
		elems[x][y+1].joint[DOWN] = elems[x][y+1].joint[UP] = elems[x][y+1].joint[RIGHT] = true;
		break;
	case DRAGON_RIGHT:
		elems[x][y].pFount = p;
		elems[x][y+1].pFount = p;
		elems[x][y].joint[LEFT] = false;
		elems[x][y].joint[RIGHT] = elems[x][y].joint[DOWN] = elems[x][y].joint[UP] = true;
		elems[x][y+1].joint[RIGHT] = false;
		elems[x][y+1].joint[DOWN] = elems[x][y+1].joint[UP] = elems[x][y+1].joint[LEFT] = true;
		break;
	case VIRNUS:
		elems[x][y].pFount = p;
		elems[x+1][y].pFount = p;
		elems[x][y].joint[DOWN] = elems[x][y].joint[UP] = elems[x][y].joint[RIGHT] = true;
		elems[x][y].joint[LEFT] = false;
		elems[x+1][y].joint[LEFT] = elems[x+1][y].joint[UP] = elems[x+1][y].joint[DOWN] = true;
		elems[x+1][y].joint[RIGHT] = false;
		break;
	}
}

void board::reset() {
	// 無効な要素をボードから取り除く
	for(uint8 i=0; i<4; ++i) {
		for(uint8 j=0; j<4; ++j) {
			elems[i][j].pFount = 0;
			elems[i][j].joint[UP] = elems[i][j].joint[RIGHT] = elems[i][j].joint[DOWN] = elems[i][j].joint[LEFT] = false;
		}
	}
	for(uint8 i=0; i<16; ++i) {
		d_fountain[i].type = NOTHING;
	}
}

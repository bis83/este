#pragma once

typedef unsigned char uint8;

// ジョイントの方角
enum joint_dir {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// 噴水の種類
enum fountain_type {
	WATER_LOAD,
	FRONT_OF_HOUSE,
	NEPTUNE,
	CIRCUIT_TOP_LEFT,
	CIRCUIT_TOP_RIGHT,
	CIRCUIT_RIGHT_DOWN,
	CIRCUIT_DOWN_LEFT,
	DRAGON_LEFT,
	DRAGON_RIGHT,
	VIRNUS,
	NOTHING				// 空の噴水
};

// 水源
struct fountainhead {
	int water;		// 水量(0~100)
	int time;

	fountainhead() : water(0), time(0) {}
};

// 噴水
struct fountain {
	fountain_type type;		// 噴水の種類
	uint8 x, y, width, height;	// x,y:噴水の左上(ボードスクエア単位)
	int score;
	bool water_f;

	fountain() : type(NOTHING), x(0), y(0), width(0), height(0), score(0), water_f(false) {}
	void set(int no);
	void set(fountain &next, uint8 x, uint8 y);
};

// ボード要素
struct board_elem {
	fountain *pFount;		// 関連する噴水へのポインタ
	bool joint[4];				// 接続フラグ

	board_elem() : pFount(0) {
		joint[UP] = joint[LEFT] = joint[RIGHT] = joint[DOWN] = false;
	}
};

// ボード
struct board {
	board_elem elems[4][4];
	fountain d_fountain[16];
	fountainhead d_fountainhead[4];
	fountain d_next[4];		// 設置
	int score;				// スコア
	bool init_f;			// 初期化フラグ
	bool prestart_f;
	bool gameover_f;
	int time;

	// 60fに一回の更新
	void update();

	// 初期化
	void init();

	// クリック時の処理
	void click();

	// ボードのリセット
	void reset();

	// 新しい噴水の設置
	void addFountain(uint8 x, uint8 y);

	// 次の噴水が設置可能か
	bool enablePut(uint8 x, uint8 y, fountain &next);

	// iとj番目の水源が結ばれているか
	bool joined(int i, int j, bool check[4][4], board_elem *elemlist[16]);
	bool joined_func(int goal, uint8 x, uint8 y, joint_dir src_dir, bool check[4][4], board_elem *elemlist[16]);

	// 描画
	void draw();

	board() : score(0), init_f(false) {}
};

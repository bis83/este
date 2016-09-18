#pragma once

typedef unsigned char uint8;

// �W���C���g�̕��p
enum joint_dir {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// �����̎��
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
	NOTHING				// ��̕���
};

// ����
struct fountainhead {
	int water;		// ����(0~100)
	int time;

	fountainhead() : water(0), time(0) {}
};

// ����
struct fountain {
	fountain_type type;		// �����̎��
	uint8 x, y, width, height;	// x,y:�����̍���(�{�[�h�X�N�G�A�P��)
	int score;
	bool water_f;

	fountain() : type(NOTHING), x(0), y(0), width(0), height(0), score(0), water_f(false) {}
	void set(int no);
	void set(fountain &next, uint8 x, uint8 y);
};

// �{�[�h�v�f
struct board_elem {
	fountain *pFount;		// �֘A���镬���ւ̃|�C���^
	bool joint[4];				// �ڑ��t���O

	board_elem() : pFount(0) {
		joint[UP] = joint[LEFT] = joint[RIGHT] = joint[DOWN] = false;
	}
};

// �{�[�h
struct board {
	board_elem elems[4][4];
	fountain d_fountain[16];
	fountainhead d_fountainhead[4];
	fountain d_next[4];		// �ݒu
	int score;				// �X�R�A
	bool init_f;			// �������t���O
	bool prestart_f;
	bool gameover_f;
	int time;

	// 60f�Ɉ��̍X�V
	void update();

	// ������
	void init();

	// �N���b�N���̏���
	void click();

	// �{�[�h�̃��Z�b�g
	void reset();

	// �V���������̐ݒu
	void addFountain(uint8 x, uint8 y);

	// ���̕������ݒu�\��
	bool enablePut(uint8 x, uint8 y, fountain &next);

	// i��j�Ԗڂ̐��������΂�Ă��邩
	bool joined(int i, int j, bool check[4][4], board_elem *elemlist[16]);
	bool joined_func(int goal, uint8 x, uint8 y, joint_dir src_dir, bool check[4][4], board_elem *elemlist[16]);

	// �`��
	void draw();

	board() : score(0), init_f(false) {}
};

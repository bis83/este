#pragma once

#include "Audio.h"

struct sound_bank {
	Stream *pMusic;
	SoundBank *pSoundBank;
	int s_id[6];

	bool init();
	void release();

	sound_bank() : pMusic(0), pSoundBank(0) {}
};

extern sound_bank sb;

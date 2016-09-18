#include "Sound.h"
#include "Game.h"

sound_bank sb;

bool sound_bank::init() {
	pMusic = new Stream(Game::GetAudio());
	pMusic->SetMusicFile(L"rsc/water.ogg", AF_OGG);

	WAVEFORMATEX format;
	DWORD size;
	AudioFile *fp = new AudioFileWave();
	fp->OpenFile(L"rsc/fountainhead.wav");
	fp->GetWaveFormat(&format, &size);
	fp->Close();
	delete fp;
	pSoundBank = new SoundBank(Game::GetAudio(), 8, format);
	s_id[0] = pSoundBank->PushWave(L"rsc/fountainhead.wav", AF_WAVE);
	s_id[1] = pSoundBank->PushWave(L"rsc/score.wav", AF_WAVE);
	s_id[2] = pSoundBank->PushWave(L"rsc/put.wav", AF_WAVE);
	s_id[3] = pSoundBank->PushWave(L"rsc/prestart.wav", AF_WAVE);
	s_id[4] = pSoundBank->PushWave(L"rsc/gameover.wav", AF_WAVE);
	s_id[5] = pSoundBank->PushWave(L"rsc/miss.wav", AF_WAVE);

	return true;
}

void sound_bank::release() {
	pSoundBank->Stop();
	delete pSoundBank;
	pMusic->Stop();
	delete pMusic;
}
#include "Audio.h"
#include "Game.h"

Audio::Audio() : m_pAudio(NULL), m_pMasterVoice(NULL) {
	if(!Initialize()) {
		MESSAGE_BOX(L"Audioの初期化に失敗");
	}
}

Audio::~Audio() {
	if(m_pMasterVoice) {
		m_pMasterVoice->DestroyVoice();
	}
	SAFE_RELEASE(m_pAudio);
}

bool Audio::Initialize() {
	// WICとの衝突。
	//CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2Engineの生成
	if(FAILED(XAudio2Create( &m_pAudio, 0, XAUDIO2_DEFAULT_PROCESSOR ))) {
		FITAL_ERROR(L"XAudio2Engineの生成に失敗");
		return false;
	}

	// マスターボイスの生成
	if(FAILED(m_pAudio->CreateMasteringVoice(&m_pMasterVoice))) {
		FITAL_ERROR(L"MasteringVoiceの生成に失敗");
		return false;
	}

	return true;
}

void Audio::SetVolume(float vol) {
	m_pMasterVoice->SetVolume(vol);
}

AudioObject::AudioObject(Audio *pAudio) : pAudio(pAudio) {
}

AudioObject::AudioObject(AudioObject &a) : pAudio(a.pAudio) {
}

AudioObject::~AudioObject() {
}


Sound::Sound(Audio *pAudio) : AudioObject(pAudio) {
}

Sound::Sound(Audio *pAudio, LPCWSTR filename, AUDIO_FILE_TYPE type) :
AudioObject(pAudio), m_pVoice(NULL), play_f(false), m_buffer() {
	if(!Initialize(filename, type)) {
		MESSAGE_BOX(L"サウンドの初期化に失敗");
	}
}

Sound::Sound(Sound &s) : AudioObject(s.pAudio), m_pVoice(s.m_pVoice), play_f(s.play_f), m_buffer(s.m_buffer) {
}

Sound::~Sound() {
	if(m_pVoice) {
		m_pVoice->FlushSourceBuffers();
		m_pVoice->DestroyVoice();
	}
	if(m_buffer.pAudioData) {
		delete[] m_buffer.pAudioData;
	}
}

bool Sound::Initialize(LPCWSTR filename, AUDIO_FILE_TYPE type) {
	AudioFile *pFile;

	switch(type) {
	case AF_WAVE:
		pFile = new AudioFileWave();
		break;
	case AF_OGG:
		pFile = new AudioFileOgg();
		break;
	}
	
	pFile->OpenFile(const_cast<LPWSTR>(filename));
	DWORD size;
	WAVEFORMATEX format;
	pFile->GetWaveFormat(&format, &size);
	
	BYTE *pBuf = new BYTE[size];
	pFile->Read(pBuf, size);

	m_buffer.Flags = 0;
	m_buffer.LoopBegin = 0;
	m_buffer.LoopCount = 0;
	m_buffer.LoopLength = 0;
	m_buffer.pAudioData = pBuf;
	m_buffer.AudioBytes = size;
	m_buffer.pContext = NULL;
	m_buffer.PlayBegin = 0;
	m_buffer.PlayLength = 0;

	pFile->Close();
	delete pFile;

	if(FAILED(pAudio->GetAudio()->CreateSourceVoice(&m_pVoice, &format))) {
		FITAL_ERROR(L"ソースボイスの生成に失敗");
		return false;
	}

	return true;
}

void Sound::SetVolume(float vol) {
	m_pVoice->SetVolume(vol);
}

void Sound::SetFreq(float ratio) {
	m_pVoice->SetFrequencyRatio(ratio);
}

void Sound::SetBlock(unsigned int begin ,unsigned int length) {
	m_buffer.PlayBegin = begin;
	m_buffer.PlayLength = length;
}

void Sound::SetLoop(unsigned int loop_count, unsigned int loop_begin, unsigned int loop_end) {
	m_buffer.LoopBegin = loop_begin;
	m_buffer.LoopCount = loop_count;
	m_buffer.LoopLength = loop_end - loop_begin;
}

void Sound::Play() {
	m_pVoice->SubmitSourceBuffer(&m_buffer);
	m_pVoice->Start();
}

void Sound::Stop() {
	m_pVoice->Stop();
}


SoundBank::SoundBank(Audio *pAudio) : AudioObject(pAudio), m_ppVoice(NULL), m_listActive(), m_listFree(), m_bufWave(), m_pCallbacks(NULL)
{
}

SoundBank::SoundBank(Audio *pAudio, UINT nPans, WAVEFORMATEX &format) : AudioObject(pAudio), nPans(0), m_ppVoice(NULL), m_listActive(), m_listFree(), m_bufWave(), m_pCallbacks(NULL)
{
	if(!Initialize(nPans, format)) {
		MESSAGE_BOX(L"サウンドバンクの初期化に失敗");
	}
}

SoundBank::~SoundBank() {
	ClearWave();
	for(UINT i=0; i<nPans; ++i) {
		m_ppVoice[i]->DestroyVoice();
	}
	if(m_ppVoice) delete[] m_ppVoice;
	if(m_pCallbacks) delete[] m_pCallbacks;
}

bool SoundBank::Initialize(UINT nPans, WAVEFORMATEX &format) {
	this->nPans = nPans;
	m_ppVoice = new IXAudio2SourceVoice*[nPans];
	m_pCallbacks = new BankCallback[nPans];
	for(UINT i=0; i<nPans; ++i) {
		m_ppVoice[i] = NULL;
		if(FAILED(pAudio->GetAudio()->CreateSourceVoice(&m_ppVoice[i], &format, 0U, 2.0f, &m_pCallbacks[i]))) {
			FITAL_ERROR(L"ソースボイスの生成に失敗");
			return false;
		}
		m_pCallbacks[i].SetParent(this);
		m_pCallbacks[i].SetVoice(m_ppVoice[i]);

		m_listFree.insert(m_ppVoice[i]);
	}
	return true;
}

int SoundBank::PushWave(LPCWSTR filepass, AUDIO_FILE_TYPE type) {
	AudioFile *pFile;
	XAUDIO2_BUFFER buf;

	switch(type) {
	case AF_WAVE:
		pFile = new AudioFileWave();
		break;
	case AF_OGG:
		pFile = new AudioFileOgg();
		break;
	}
	
	pFile->OpenFile(const_cast<LPWSTR>(filepass));
	DWORD size;
	WAVEFORMATEX format;
	pFile->GetWaveFormat(&format, &size);
	
	BYTE *pBuf = new BYTE[size];
	pFile->Read(pBuf, size);

	buf.Flags = 0;
	buf.LoopBegin = 0;
	buf.LoopCount = 0;
	buf.LoopLength = 0;
	buf.pAudioData = pBuf;
	buf.AudioBytes = size;
	buf.pContext = NULL;
	buf.PlayBegin = 0;
	buf.PlayLength = 0;

	pFile->Close();
	delete pFile;

	m_bufWave.push_back(buf);
	return m_bufWave.size()-1;
}

void SoundBank::PopWave(int id) {
	delete m_bufWave[id].pAudioData;
	vector<XAUDIO2_BUFFER>::iterator i(&m_bufWave[id], &m_bufWave);
	m_bufWave.erase(i);
}

void SoundBank::ClearWave() {
	for(vector<XAUDIO2_BUFFER>::iterator i=m_bufWave.begin(); i!=m_bufWave.end(); ++i) {
		delete i->pAudioData;
	}
	m_bufWave.clear();
}

void SoundBank::SetBlock(int id, unsigned int begin, unsigned int length) {
	m_bufWave[id].PlayBegin = begin;
	m_bufWave[id].PlayLength = length;
}

void SoundBank::SetLoop(int id, unsigned int loop_count, unsigned int loop_begin, unsigned int loop_end) {
	m_bufWave[id].LoopCount = loop_count;
	m_bufWave[id].LoopBegin = loop_begin;
	m_bufWave[id].LoopLength = loop_end - loop_begin;
}

void SoundBank::Play(int id, float vol, float freq) {
	if(!m_listFree.empty()) {
		// タスクを入手する
		IXAudio2SourceVoice *ref = *m_listFree.begin();

		// 再生の設定をする
		ref->SetVolume(vol);
		ref->SetFrequencyRatio(freq);
		ref->SubmitSourceBuffer(&m_bufWave[id]);
		ref->Start();
	}
}

void SoundBank::Stop() {
	while(!m_listActive.empty()) {
		IXAudio2SourceVoice *ref = *m_listActive.begin();
		m_listActive.erase(ref);

		ref->Stop();
		ref->FlushSourceBuffers();
	}
}

void SoundBank::BankCallback::OnBufferStart(void *pContext) {
	// タスクをアクティブにする
	pParent->m_listActive.insert(pVoice);
	pParent->m_listFree.erase(pVoice);
}

void SoundBank::BankCallback::OnBufferEnd(void *pContext) {
	// フリータスクを増やす
	pParent->m_listActive.erase(pVoice);
	pParent->m_listFree.insert(pVoice);
}


Stream::Stream(Audio *pAudio) :
AudioObject(pAudio), m_pVoice(NULL), buffer(), block(), b_counter(0), pFile(NULL), callback(this), play_f(false) {
	if(!Initialize()) {
		MESSAGE_BOX(L"ストリームの初期化に失敗");
	}
}

Stream::Stream(Stream &s) :
AudioObject(pAudio), m_pVoice(s.m_pVoice), buffer(s.buffer),
block(), b_counter(0), pFile(s.pFile), callback(this)
{
}

Stream::~Stream() {
	if(m_pVoice) {
		m_pVoice->DestroyVoice();
	}
	if(pFile) {
		delete pFile;
	}
	for(int i=0; i<4; ++i) {
		if(block[i].p) {
			delete[] block[i].p;
			block[i].p = NULL;
		}
	}
}

bool Stream::Initialize() {
	return true;
}

bool Stream::SetMusicFile(LPCWSTR filename, AUDIO_FILE_TYPE type) {

	// 再生中の場合は中断させる
	if(play_f) {
		Stop();
		// 停止が正常に終了するように待つ
		Sleep(1);
	}

	// 前のミュージックファイルが設定されている場合は
	// それを解放する
	if(pFile) {
		m_pVoice->DestroyVoice();
		m_pVoice = NULL;

		for(int i=0; i<4; ++i) {
			if(block[i].p) {
				delete[] block[i].p;
				block[i].p = NULL;
			}
		}

		delete pFile;
		pFile = NULL;
	}

	switch(type) {
	case AF_WAVE:
		pFile = new AudioFileWave();
		break;
	case AF_OGG:
		pFile = new AudioFileOgg();
		break;
	}

	pFile->OpenFile(const_cast<LPWSTR>(filename));
	DWORD size;
	WAVEFORMATEX format;
	pFile->GetWaveFormat(&format, &size);

	pFile->SetLoop(0, size / format.nBlockAlign);

	for(int i=0; i<4; ++i) {
		block[i].p = new BYTE[format.nSamplesPerSec * format.nBlockAlign];
		block[i].size = format.nSamplesPerSec * format.nBlockAlign;
	}

	buffer.Flags = 0;
	buffer.LoopBegin = 0;
	buffer.LoopCount = 0;
	buffer.LoopLength = 0;
	buffer.pAudioData = NULL;
	buffer.AudioBytes = 0;
	buffer.pContext = NULL;
	buffer.PlayBegin = 0;
	buffer.PlayLength = 0;

	if(FAILED(pAudio->GetAudio()->CreateSourceVoice(&m_pVoice, &format, 0U, 2.0f, &callback))) {
		FITAL_ERROR(L"ソースボイスの生成に失敗");
		return false;
	}
}

void Stream::SetVolume(float vol) {
	m_pVoice->SetVolume(vol);
}

void Stream::SetFreq(float ratio) {
	m_pVoice->SetFrequencyRatio(ratio);
}

void Stream::SetLoop(unsigned int loop_begin, unsigned int loop_end) {
	pFile->SetLoop(loop_begin, loop_end);
}

void Stream::Play() {
	m_pVoice->FlushSourceBuffers();
	for(int i=0; i<3; ++i) {
		pFile->ReadLoop(block[i].p, block[i].size);
		buffer.pAudioData = block[i].p;
		buffer.AudioBytes = block[i].size;
		m_pVoice->SubmitSourceBuffer(&buffer);
	}
	b_counter = 3;
	m_pVoice->Start();
}

void Stream::Stop() {
	m_pVoice->Stop();
}

void Stream::StreamCallback::OnBufferStart(void *pContext) {
	int i = pParent->b_counter;

	pParent->pFile->ReadLoop(pParent->block[i].p, pParent->block[i].size);
	pParent->buffer.pAudioData = pParent->block[i].p;
	pParent->buffer.AudioBytes = pParent->block[i].size;
	pParent->m_pVoice->SubmitSourceBuffer(&pParent->buffer);
	
	pParent->b_counter++;
	if(pParent->b_counter > 3) {
		pParent->b_counter = 0;
	}
}

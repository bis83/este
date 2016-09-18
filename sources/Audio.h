#pragma once

#include <XAudio2.h>
//#pragma comment(lib, "XAudio2.lib")

#include "AudioFile.h"

#include <list>
#include <set>
#include <vector>
using namespace std;

//=============================================================
// Class : Audio
//-------------------------------------------------------------
// 音響システムクラス。
//=============================================================
class Audio {
	Audio(Audio &) {}

	IXAudio2 *m_pAudio;
	IXAudio2MasteringVoice *m_pMasterVoice;
public:
	Audio();
	~Audio();

	// 初期化
	bool Initialize();

	// マスターボリュームの設定
	void SetVolume(float vol);

	// 各XAudio2Objectの取得
	IXAudio2 *GetAudio() { return m_pAudio; }
	IXAudio2MasteringVoice *GetMasteringVoice() { m_pMasterVoice; }
};

class AudioObject {
protected:
	Audio *pAudio;
public:
	AudioObject(Audio *pAudio);
	AudioObject(AudioObject &);
	~AudioObject();
};

//=============================================================
// Class : Sound
//-------------------------------------------------------------
// 効果音等再生クラス。
//=============================================================
class Sound  : public AudioObject {
	bool play_f;

	IXAudio2SourceVoice *m_pVoice;
	XAUDIO2_BUFFER m_buffer;
public:
	Sound(Audio *pAudio);
	Sound(Audio *pAudio, LPCWSTR filename, AUDIO_FILE_TYPE type);
	Sound(Sound &);
	~Sound();

	// 初期化
	bool Initialize(LPCWSTR filename, AUDIO_FILE_TYPE type);

	// 音量の設定
	void SetVolume(float vol);
	// 再生周波数比の設定
	void SetFreq(float ratio);
	// 再生範囲の設定(再生前にのみ有効, pcm単位)
	void SetBlock(unsigned int begin, unsigned int length);
	// ループ情報の設定(再生前にのみ有効, pcm単位)
	void SetLoop(unsigned int loop_count, unsigned int loop_begin, unsigned int loop_end);

	// 再生
	void Play();
	// 停止
	void Stop();

	// 各XAudio2Objectの取得
	IXAudio2SourceVoice *GetVoice() { return m_pVoice; }
};

//=============================================================
// Class : SoundBank
//-------------------------------------------------------------
// 効果音集を再生するクラス。
//=============================================================
class SoundBank : public AudioObject {
	// ボイス
	IXAudio2SourceVoice **m_ppVoice;
	set<IXAudio2SourceVoice*> m_listActive, m_listFree;

	// バッファ
	vector<XAUDIO2_BUFFER> m_bufWave;

	// バンク再生用コールバック
	class BankCallback : public IXAudio2VoiceCallback {
	protected:
		SoundBank *pParent;
		IXAudio2SourceVoice *pVoice;
	public:
		BankCallback() : pParent(NULL), pVoice(NULL) {}
		void SetParent(SoundBank *p) {
			pParent = p;
		}
		void SetVoice(IXAudio2SourceVoice *p) {
			pVoice = p;
		}

		STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 ) { }
        STDMETHOD_( void, OnVoiceProcessingPassEnd )() { }
        STDMETHOD_( void, OnStreamEnd )() { }
        STDMETHOD_( void, OnBufferStart )( void* );
        STDMETHOD_( void, OnBufferEnd )( void* );
        STDMETHOD_( void, OnLoopEnd )( void* ) { }
        STDMETHOD_( void, OnVoiceError )( void*, HRESULT ) { }
	};
	friend class BankCallback;
	BankCallback *m_pCallbacks;
	UINT nPans;
public:
	SoundBank(Audio *pAudio);
	SoundBank(Audio *pAudio, UINT nPans, WAVEFORMATEX &format);
	~SoundBank();

	// 初期化
	bool Initialize(UINT nPans, WAVEFORMATEX &format);

	// サウンドの登録と削除(pushしたときにidを返す)
	int PushWave(LPCWSTR filepass, AUDIO_FILE_TYPE type);
	void PopWave(int id);
	void ClearWave();

	// サウンドの設定
	void SetBlock(int id, unsigned int begin, unsigned int length);
	void SetLoop(int id, unsigned int loop_count, unsigned int loop_begin, unsigned int loop_end);

	// 再生
	void Play(int id, float vol = 1.0f, float freq = 1.0f);
	// 停止
	void Stop();
};

//=============================================================
// Class : Stream
//-------------------------------------------------------------
// ストリーム再生クラス。
//=============================================================
class Stream : public AudioObject {
	bool play_f;

	IXAudio2SourceVoice *m_pVoice;

	XAUDIO2_BUFFER buffer;
	
	// バッファーブロック
	struct Block {
		unsigned char *p;		// ブロックへのポインタ
		unsigned int size;		// ブロックのサイズ
	} block[4];
	int b_counter;

	AudioFile *pFile;

	// ストリーム再生用コールバック
	class StreamCallback : public IXAudio2VoiceCallback {
	protected:
		Stream *pParent;
	public:
		StreamCallback(Stream *p) : pParent(p) {}

		STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 ) { }
        STDMETHOD_( void, OnVoiceProcessingPassEnd )() { }
        STDMETHOD_( void, OnStreamEnd )() { }
        STDMETHOD_( void, OnBufferStart )( void* );
		STDMETHOD_( void, OnBufferEnd )( void* ) { }
        STDMETHOD_( void, OnLoopEnd )( void* ) { }
        STDMETHOD_( void, OnVoiceError )( void*, HRESULT ) { }
	} callback;
	friend class StreamCallback;

public:
	Stream(Audio *pAudio);
	Stream(Stream &);
	~Stream();

	// 初期化
	bool Initialize();

	// 楽曲の設定(再生前にのみ有効)
	bool SetMusicFile(LPCWSTR filename, AUDIO_FILE_TYPE type);

	// 音量の設定
	void SetVolume(float vol);
	// 再生周波数比の設定
	void SetFreq(float ratio);

	// ループの設定(再生前にのみ有効)
	void SetLoop(unsigned int loop_begin, unsigned int loop_end);

	// 再生
	void Play();
	// 停止
	void Stop();
};

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
// �����V�X�e���N���X�B
//=============================================================
class Audio {
	Audio(Audio &) {}

	IXAudio2 *m_pAudio;
	IXAudio2MasteringVoice *m_pMasterVoice;
public:
	Audio();
	~Audio();

	// ������
	bool Initialize();

	// �}�X�^�[�{�����[���̐ݒ�
	void SetVolume(float vol);

	// �eXAudio2Object�̎擾
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
// ���ʉ����Đ��N���X�B
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

	// ������
	bool Initialize(LPCWSTR filename, AUDIO_FILE_TYPE type);

	// ���ʂ̐ݒ�
	void SetVolume(float vol);
	// �Đ����g����̐ݒ�
	void SetFreq(float ratio);
	// �Đ��͈͂̐ݒ�(�Đ��O�ɂ̂ݗL��, pcm�P��)
	void SetBlock(unsigned int begin, unsigned int length);
	// ���[�v���̐ݒ�(�Đ��O�ɂ̂ݗL��, pcm�P��)
	void SetLoop(unsigned int loop_count, unsigned int loop_begin, unsigned int loop_end);

	// �Đ�
	void Play();
	// ��~
	void Stop();

	// �eXAudio2Object�̎擾
	IXAudio2SourceVoice *GetVoice() { return m_pVoice; }
};

//=============================================================
// Class : SoundBank
//-------------------------------------------------------------
// ���ʉ��W���Đ�����N���X�B
//=============================================================
class SoundBank : public AudioObject {
	// �{�C�X
	IXAudio2SourceVoice **m_ppVoice;
	set<IXAudio2SourceVoice*> m_listActive, m_listFree;

	// �o�b�t�@
	vector<XAUDIO2_BUFFER> m_bufWave;

	// �o���N�Đ��p�R�[���o�b�N
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

	// ������
	bool Initialize(UINT nPans, WAVEFORMATEX &format);

	// �T�E���h�̓o�^�ƍ폜(push�����Ƃ���id��Ԃ�)
	int PushWave(LPCWSTR filepass, AUDIO_FILE_TYPE type);
	void PopWave(int id);
	void ClearWave();

	// �T�E���h�̐ݒ�
	void SetBlock(int id, unsigned int begin, unsigned int length);
	void SetLoop(int id, unsigned int loop_count, unsigned int loop_begin, unsigned int loop_end);

	// �Đ�
	void Play(int id, float vol = 1.0f, float freq = 1.0f);
	// ��~
	void Stop();
};

//=============================================================
// Class : Stream
//-------------------------------------------------------------
// �X�g���[���Đ��N���X�B
//=============================================================
class Stream : public AudioObject {
	bool play_f;

	IXAudio2SourceVoice *m_pVoice;

	XAUDIO2_BUFFER buffer;
	
	// �o�b�t�@�[�u���b�N
	struct Block {
		unsigned char *p;		// �u���b�N�ւ̃|�C���^
		unsigned int size;		// �u���b�N�̃T�C�Y
	} block[4];
	int b_counter;

	AudioFile *pFile;

	// �X�g���[���Đ��p�R�[���o�b�N
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

	// ������
	bool Initialize();

	// �y�Ȃ̐ݒ�(�Đ��O�ɂ̂ݗL��)
	bool SetMusicFile(LPCWSTR filename, AUDIO_FILE_TYPE type);

	// ���ʂ̐ݒ�
	void SetVolume(float vol);
	// �Đ����g����̐ݒ�
	void SetFreq(float ratio);

	// ���[�v�̐ݒ�(�Đ��O�ɂ̂ݗL��)
	void SetLoop(unsigned int loop_begin, unsigned int loop_end);

	// �Đ�
	void Play();
	// ��~
	void Stop();
};

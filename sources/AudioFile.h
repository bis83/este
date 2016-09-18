#pragma once

#include "vorbis/vorbisfile.h"
#include <XAudio2.h>

#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")

#pragma comment(lib, "winmm.lib")

//=============================================================
// enum : AUDIO_FILE_TYPE
//-------------------------------------------------------------
// �I�[�f�B�I�t�@�C���̌`���B
// �I�[�f�B�I�t�@�C���̓ǂݍ��ݕ��@��A�f�R�[�h�������ω�����B
//=============================================================
enum AUDIO_FILE_TYPE {
	AF_WAVE,		// wave�t�H�[�}�b�g
	AF_OGG			// ogg vorbis�t�H�[�}�b�g
};

//=============================================================
// Class : AudioFile
//-------------------------------------------------------------
// �I�[�f�B�I�t�@�C���̊���B
//=============================================================
class AudioFile {
public:
	// �t�@�C�����J��
	virtual bool OpenFile(LPWSTR filename) = 0;
	// �t�@�C�������
	virtual void Close() = 0;
	// �t�H�[�}�b�g�̎擾(�T�C�Y�̓o�C�g�P��)
	virtual void GetWaveFormat(WAVEFORMATEX *pFormat, DWORD *pSize) = 0;

	// �V�[�N(pcm�P��)
	virtual void Seek(UINT position) = 0;

	// �o��(�o�C�g�P��)
	virtual void Read(BYTE *pOutput, UINT size) = 0;
	
	// ���[�v�o��(�Z�b�g��pcm�P��, �o�͂̓o�C�g�P��)
	virtual void SetLoop(DWORD begin, DWORD end) = 0;
	virtual void ReadLoop(BYTE *pOutput, UINT size) = 0;
};

//=============================================================
// Class : AudioFileWave
//-------------------------------------------------------------
// Wave�`���̃I�[�f�B�I�t�@�C���B
//=============================================================
class AudioFileWave : public AudioFile {
	// �f�[�^
	WAVEFORMATEX format;
	DWORD wavSize;
	DWORD offset;

	// �t�@�C��
	FILE *fp;

	// ���[�v���
	DWORD b, e;
	DWORD wavSeek;
public:
	bool OpenFile(LPWSTR filename);
	void Close();
	void GetWaveFormat(WAVEFORMATEX *pFormat, DWORD *pSize);
	void Seek(UINT position);
	void Read(BYTE *pOutput, UINT size);
	void SetLoop(DWORD begin, DWORD end);
	void ReadLoop(BYTE *pOutput, UINT size);
};

//=============================================================
// Class : AudioFileOgg
//-------------------------------------------------------------
// ogg vorbis�`���̃I�[�f�B�I�t�@�C���B
//=============================================================
class AudioFileOgg : public AudioFile {
	// �f�[�^
	WAVEFORMATEX format;
	DWORD wavSize;

	// �t�@�C��
	FILE *fp;
	OggVorbis_File vf;

	// ���[�v���
	DWORD b, e;
	DWORD wavSeek;
public:
	bool OpenFile(LPWSTR filename);
	void Close();
	void GetWaveFormat(WAVEFORMATEX *pFormat, DWORD *pSize);
	void Seek(UINT position);
	void Read(BYTE *pOutput, UINT size);
	void SetLoop(DWORD begin, DWORD end);
	void ReadLoop(BYTE *pOutput, UINT size);
};

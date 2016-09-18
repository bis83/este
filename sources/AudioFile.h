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
// オーディオファイルの形式。
// オーディオファイルの読み込み方法や、デコード処理が変化する。
//=============================================================
enum AUDIO_FILE_TYPE {
	AF_WAVE,		// waveフォーマット
	AF_OGG			// ogg vorbisフォーマット
};

//=============================================================
// Class : AudioFile
//-------------------------------------------------------------
// オーディオファイルの既定。
//=============================================================
class AudioFile {
public:
	// ファイルを開く
	virtual bool OpenFile(LPWSTR filename) = 0;
	// ファイルを閉じる
	virtual void Close() = 0;
	// フォーマットの取得(サイズはバイト単位)
	virtual void GetWaveFormat(WAVEFORMATEX *pFormat, DWORD *pSize) = 0;

	// シーク(pcm単位)
	virtual void Seek(UINT position) = 0;

	// 出力(バイト単位)
	virtual void Read(BYTE *pOutput, UINT size) = 0;
	
	// ループ出力(セットはpcm単位, 出力はバイト単位)
	virtual void SetLoop(DWORD begin, DWORD end) = 0;
	virtual void ReadLoop(BYTE *pOutput, UINT size) = 0;
};

//=============================================================
// Class : AudioFileWave
//-------------------------------------------------------------
// Wave形式のオーディオファイル。
//=============================================================
class AudioFileWave : public AudioFile {
	// データ
	WAVEFORMATEX format;
	DWORD wavSize;
	DWORD offset;

	// ファイル
	FILE *fp;

	// ループ情報
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
// ogg vorbis形式のオーディオファイル。
//=============================================================
class AudioFileOgg : public AudioFile {
	// データ
	WAVEFORMATEX format;
	DWORD wavSize;

	// ファイル
	FILE *fp;
	OggVorbis_File vf;

	// ループ情報
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

#include "AudioFile.h"
#include "Game.h"

bool AudioFileWave::OpenFile(LPWSTR filename) {
	HMMIO hMmio = NULL;
	hMmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if((fp = _wfopen(filename, L"rb")) == 0) {
		FITAL_FILE_ERROR(filename);
		return false;
	}

	MMCKINFO riffckInfo;
	if(MMSYSERR_NOERROR != mmioDescend(hMmio, &riffckInfo, NULL, 0)) {
		return false;
	}
	if((riffckInfo.ckid != mmioFOURCC('R','I','F','F'))||(riffckInfo.fccType != mmioFOURCC('W','A','V','E'))) {
		return false;
	}

	MMCKINFO ckInfo;

	ckInfo.ckid = mmioFOURCC('f','m','t',' ');
	if(MMSYSERR_NOERROR != mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK)) {
		return false;
	}
	if(mmioRead(hMmio, (HPSTR)&format, sizeof(WAVEFORMATEX)) != sizeof(WAVEFORMATEX)) {
		return false;
	}
	if(format.wFormatTag != WAVE_FORMAT_PCM) {
		return false;
	}
	if(MMSYSERR_NOERROR != mmioAscend(hMmio, &ckInfo, 0)) {
		return false;
	}
	ckInfo.ckid = mmioFOURCC('d','a','t','a');
	if(MMSYSERR_NOERROR != mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK)) {
		return false;
	}
	wavSize = ckInfo.cksize;
	offset = ckInfo.dwDataOffset;

	mmioClose(hMmio, NULL);

	Seek(0);

	return true;
}

void AudioFileWave::Close() {
	fclose(fp);
}

void AudioFileWave::GetWaveFormat(WAVEFORMATEX *pFormat, DWORD *pSize) {
	*pFormat = format;
	*pSize = wavSize;
}

void AudioFileWave::Seek(UINT position) {
	wavSeek = position * format.nBlockAlign / format.nChannels + offset;
	fseek(fp, wavSeek, SEEK_SET);
}

void AudioFileWave::Read(BYTE *pOutput, UINT size) {
	fread(pOutput, size, 1, fp);
	wavSeek += size;
}

void AudioFileWave::SetLoop(DWORD begin, DWORD end) {
	b = begin;
	e = end;
}

void AudioFileWave::ReadLoop(BYTE *pOutput, UINT size) {
	// ループ終端を越す場合
	if(e * format.nBlockAlign < wavSeek + size) {
		// 2回に分けてリードを行う
		DWORD d = wavSeek + size - e * format.wBitsPerSample;
		Read(pOutput, d);
		pOutput += d;
		Seek(b);
		Read(pOutput, size - d);
	} else {
		// そうでない場合は1回でリードを行う
		Read(pOutput, size);
	}
}

bool AudioFileOgg::OpenFile(LPWSTR filename) {
	if((fp = _wfopen(filename, L"rb")) != 0) {
		if(ov_open(fp, &vf, NULL, 0) == 0) {
			vorbis_info *vi;
			vi = ov_info(&vf, -1);
			format.cbSize = sizeof(WAVEFORMATEX);
			format.nChannels = vi->channels;
			format.nSamplesPerSec = vi->rate;
			format.wBitsPerSample = 16;
			format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
			format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;
			format.wFormatTag = WAVE_FORMAT_PCM;
			wavSize = (DWORD)(max(ov_pcm_total(&vf, -1), 0)) * format.nBlockAlign;

			Seek(0);

			return true;
		}
	} else {
		FITAL_FILE_ERROR(filename);
	}
	return false;
}

void AudioFileOgg::Close() {
	ov_clear(&vf);
	fclose(fp);
}

void AudioFileOgg::GetWaveFormat(WAVEFORMATEX *pFormat, DWORD *pSize) {
	*pFormat = format;
	*pSize = wavSize;
}

void AudioFileOgg::Seek(UINT position) {
	ov_pcm_seek(&vf, position);
	wavSeek = position * format.nBlockAlign;
}

void AudioFileOgg::Read(BYTE *pOutput, UINT size) {
	wavSeek += size;
	int read = 0;
	do {
		read = ov_read(&vf, (char*)pOutput, size, 0, 2, 1, NULL);
		pOutput = (BYTE*)((char*)pOutput + read);
		size -= read;
	} while(read > 0);
}

void AudioFileOgg::SetLoop(DWORD begin, DWORD end) {
	b = begin;
	e = end;
}

void AudioFileOgg::ReadLoop(BYTE *pOutput, UINT size) {
	if(e * format.nBlockAlign < wavSeek + size) {
		DWORD d = wavSeek + size - e * format.nBlockAlign;
		Read(pOutput, d);
		pOutput += d;
		Seek(b);
		Read(pOutput, size - d);
	} else {
		Read(pOutput, size);
	}
}
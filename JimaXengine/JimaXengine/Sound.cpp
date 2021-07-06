#include "Sound.h"
#include <cassert>

Sound::Sound()
{
}

void Sound::Initialize()
{
	HRESULT result;
	// インスタンス生成
	result = XAudio2Create(xAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result)) {
		assert(0);
	}

	// マスターボイス生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	if (FAILED(result)) {
		assert(0);
	}


}

void Sound::LoadFile(const char* filename)
{
	// ファイルオープン
	std::ifstream file;
	//
	file.open(filename, std::ios_base::binary); 
	if (file.fail()) {
		// 失敗
		assert(0);
	}

	// .wavの読み込み
	// RIFF
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// riffかチェック
	if (strncmp(riff.chuc.id, "RIFF", 4) != 0) {
		assert(0);
	}

	// FORMAT
	file.read((char*)&format, sizeof(format));

	// DATA
	file.read((char*)&data, sizeof(data));

	// Dataチャンクのデータ部
	pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();
}

void Sound::Play()
{
	HRESULT result;
	WAVEFORMATEX wfex{};
	// 波形フォーマットの設定
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	// 波形フォーマットを元にSouceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if (FAILED(result)) {
		delete[] pBuffer;
		return;
	}
	// 再生するデータの設定
	XAUDIO2_BUFFER buff{};
	buff.pAudioData = (BYTE*)pBuffer;
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.AudioBytes = data.size;

	// 再生
	result = pSourceVoice->SubmitSourceBuffer(&buff);
	result = pSourceVoice->Start();
}

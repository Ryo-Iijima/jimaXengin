#include "Sound.h"
#include <cassert>

using namespace Microsoft::WRL;
using namespace std;

string JimaXengine::Sound::soundpass="Resources/sound/";

ComPtr<IXAudio2> JimaXengine::Sound::xAudio2;
IXAudio2MasteringVoice* JimaXengine::Sound::masterVoice;

unordered_map<std::string, JimaXengine::Sound::SoundData> JimaXengine::Sound::soundDataBuffer;

JimaXengine::Sound::SoundData JimaXengine::Sound::GetSoundDataBuffer(const std::string& filename)
{
	return soundDataBuffer[filename];
}

JimaXengine::Sound::Sound()
{
}

void JimaXengine::Sound::Initialize()
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

void JimaXengine::Sound::LoadWav(const std::string& filename)
{
	ComPtr<IXAudio2> txAudio2;
	FormatChunc tFormat;
	Chunk tData;
	char* tpBuffer;

	// ファイルオープン
	std::ifstream file;
	//
	file.open(soundpass+filename, std::ios_base::binary);
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
	file.read((char*)&tFormat, sizeof(tFormat));

	// DATA
	file.read((char*)&tData, sizeof(tData));

	// Dataチャンクのデータ部
	tpBuffer = new char[tData.size];
	file.read(tpBuffer, tData.size);

	file.close();

	SoundData registData;
	registData.format = tFormat;
	registData.data = tData;
	registData.pBuffer = tpBuffer;

	soundDataBuffer.emplace(filename, registData);
}

void JimaXengine::Sound::PlayforBuff(const std::string& filename)
{
	FormatChunc tFormat = GetSoundDataBuffer(filename).format;
	Chunk tData = GetSoundDataBuffer(filename).data;
	char* tpBuffer = GetSoundDataBuffer(filename).pBuffer;

	HRESULT result;
	WAVEFORMATEX wfex{};
	// 波形フォーマットの設定
	memcpy(&wfex, &tFormat.fmt, sizeof(tFormat.fmt));
	wfex.wBitsPerSample = tFormat.fmt.nBlockAlign * 8 / tFormat.fmt.nChannels;

	// 波形フォーマットを元にSouceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if (FAILED(result)) {
		delete[] tpBuffer;
		return;
	}
	// 再生するデータの設定
	XAUDIO2_BUFFER buff{};
	buff.pAudioData = (BYTE*)tpBuffer;
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.AudioBytes = tData.size;

	// 再生
	result = pSourceVoice->SubmitSourceBuffer(&buff);
	result = pSourceVoice->Start();
}

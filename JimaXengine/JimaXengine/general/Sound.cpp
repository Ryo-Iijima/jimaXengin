#include "Sound.h"
#include <cassert>

using namespace Microsoft::WRL;
using namespace std;

string JimaXengine::Sound::soundpass="Resources/sound/";

ComPtr<IXAudio2> JimaXengine::Sound::xAudio2;
IXAudio2MasteringVoice* JimaXengine::Sound::masterVoice;

unordered_map<string, JimaXengine::Sound::FormatChunc> JimaXengine::Sound::formatBuffer;
unordered_map<string, JimaXengine::Sound::Chunk> JimaXengine::Sound::dataBuffer;
unordered_map<string, char*> JimaXengine::Sound::pBuffers;


JimaXengine::Sound::FormatChunc JimaXengine::Sound::GetFormatBuffer(const std::string& filename)
{
	return formatBuffer[filename];
}

JimaXengine::Sound::Chunk JimaXengine::Sound::GetDataBuffer(const std::string& filename)
{
	return dataBuffer[filename];
}

char* JimaXengine::Sound::GetpBuffers(const std::string& filename)
{
	return pBuffers[filename];
}

JimaXengine::Sound::Sound()
{
}

void JimaXengine::Sound::Initialize()
{
	HRESULT result;
	// �C���X�^���X����
	result = XAudio2Create(xAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result)) {
		assert(0);
	}

	// �}�X�^�[�{�C�X����
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

	// �t�@�C���I�[�v��
	std::ifstream file;
	//
	file.open(soundpass+filename, std::ios_base::binary);
	if (file.fail()) {
		// ���s
		assert(0);
	}

	// .wav�̓ǂݍ���
	// RIFF
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// riff���`�F�b�N
	if (strncmp(riff.chuc.id, "RIFF", 4) != 0) {
		assert(0);
	}

	// FORMAT
	file.read((char*)&tFormat, sizeof(tFormat));

	// DATA
	file.read((char*)&tData, sizeof(tData));

	// Data�`�����N�̃f�[�^��
	tpBuffer = new char[tData.size];
	file.read(tpBuffer, tData.size);

	file.close();

	formatBuffer.emplace(filename, tFormat);
	dataBuffer.emplace(filename, tData);
	pBuffers.emplace(filename, tpBuffer);
}

void JimaXengine::Sound::PlayforBuff(const std::string& filename)
{
	FormatChunc tFormat = GetFormatBuffer(filename);
	Chunk tData = GetDataBuffer(filename);
	char* tpBuffer = GetpBuffers(filename);

	HRESULT result;
	WAVEFORMATEX wfex{};
	// �g�`�t�H�[�}�b�g�̐ݒ�
	memcpy(&wfex, &tFormat.fmt, sizeof(tFormat.fmt));
	wfex.wBitsPerSample = tFormat.fmt.nBlockAlign * 8 / tFormat.fmt.nChannels;

	// �g�`�t�H�[�}�b�g������SouceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if (FAILED(result)) {
		delete[] tpBuffer;
		return;
	}
	// �Đ�����f�[�^�̐ݒ�
	XAUDIO2_BUFFER buff{};
	buff.pAudioData = (BYTE*)tpBuffer;
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.AudioBytes = tData.size;

	// �Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buff);
	result = pSourceVoice->Start();
}

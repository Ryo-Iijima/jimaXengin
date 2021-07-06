#include "Sound.h"
#include <cassert>

Sound::Sound()
{
}

void Sound::Initialize()
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

void Sound::LoadFile(const char* filename)
{
	// �t�@�C���I�[�v��
	std::ifstream file;
	//
	file.open(filename, std::ios_base::binary); 
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
	file.read((char*)&format, sizeof(format));

	// DATA
	file.read((char*)&data, sizeof(data));

	// Data�`�����N�̃f�[�^��
	pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();
}

void Sound::Play()
{
	HRESULT result;
	WAVEFORMATEX wfex{};
	// �g�`�t�H�[�}�b�g�̐ݒ�
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	// �g�`�t�H�[�}�b�g������SouceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if (FAILED(result)) {
		delete[] pBuffer;
		return;
	}
	// �Đ�����f�[�^�̐ݒ�
	XAUDIO2_BUFFER buff{};
	buff.pAudioData = (BYTE*)pBuffer;
	buff.Flags = XAUDIO2_END_OF_STREAM;
	buff.AudioBytes = data.size;

	// �Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buff);
	result = pSourceVoice->Start();
}

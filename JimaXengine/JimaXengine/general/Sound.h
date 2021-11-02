/// ���ʉ��Ƃ�BGM�Ƃ����֌W�̃N���X
#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <cstdint>
#include <fstream>

#pragma comment(lib,"xaudio2.lib")

namespace JimaXengine
{
	class Sound
	{
	private:	// �G�C���A�X
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:	// �T�u�N���X
		// �`�����N�w�b�_
		struct Chunk
		{
			char id[4];		// �`�����N���Ƃ�ID
			int32_t size;	// �`�����N�T�C�Y
		};
		// RIFF�w�b�_
		struct RiffHeader
		{
			Chunk chuc;		// "RIFF"
			char type[4];	// "WAVE"
		};
		// FMT�`�����N
		struct FormatChunc
		{
			Chunk chunk;	// "fmt"
			WAVEFORMAT fmt;	// �g�`�t�H�[�}�b�g
		};

	private:	// �ϐ�
		ComPtr<IXAudio2> xAudio2;
		IXAudio2MasteringVoice* masterVoice = nullptr;

		FormatChunc format;
		Chunk data;
		char* pBuffer;

	public:		// �֐�
		Sound();

		void Initialize();

		// ���f�[�^�̓ǂݍ���
		void LoadFile(const char* filename);

		// �Đ�
		void Play();

	};
}
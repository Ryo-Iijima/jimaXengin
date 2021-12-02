/// ���ʉ��Ƃ�BGM�Ƃ����֌W�̃N���X
#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <cstdint>
#include <fstream>
#include <unordered_map>

#pragma comment(lib,"xaudio2.lib")

namespace JimaXengine
{
	class Sound
	{
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
		static std::string soundpass;

		static Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
		static IXAudio2MasteringVoice* masterVoice;

		// wav�o�b�t�@
		struct SoundData
		{
			FormatChunc format;
			Chunk data;
			char* pBuffer;
		};
		static std::unordered_map<std::string, SoundData> soundDataBuffer;

		static SoundData GetSoundDataBuffer(const std::string& filename);

	public:		// �֐�
		Sound();

		static void Initialize();

		// �o�b�t�@��wav�o�^
		static void LoadWav(const std::string& filename);

		// �Đ�
		void PlayforBuff(const std::string& filename);

	};
}
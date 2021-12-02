/// 効果音とかBGMとか音関係のクラス
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
	private:	// サブクラス
		// チャンクヘッダ
		struct Chunk
		{
			char id[4];		// チャンクごとのID
			int32_t size;	// チャンクサイズ
		};
		// RIFFヘッダ
		struct RiffHeader
		{
			Chunk chuc;		// "RIFF"
			char type[4];	// "WAVE"
		};
		// FMTチャンク
		struct FormatChunc
		{
			Chunk chunk;	// "fmt"
			WAVEFORMAT fmt;	// 波形フォーマット
		};

	private:	// 変数
		static std::string soundpass;

		static Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
		static IXAudio2MasteringVoice* masterVoice;

		// wavバッファ
		struct SoundData
		{
			FormatChunc format;
			Chunk data;
			char* pBuffer;
		};
		static std::unordered_map<std::string, SoundData> soundDataBuffer;

		static SoundData GetSoundDataBuffer(const std::string& filename);

	public:		// 関数
		Sound();

		static void Initialize();

		// バッファにwav登録
		static void LoadWav(const std::string& filename);

		// 再生
		void PlayforBuff(const std::string& filename);

	};
}
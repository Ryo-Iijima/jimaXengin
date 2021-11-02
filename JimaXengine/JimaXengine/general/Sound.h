/// 効果音とかBGMとか音関係のクラス
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
	private:	// エイリアス
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

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
		ComPtr<IXAudio2> xAudio2;
		IXAudio2MasteringVoice* masterVoice = nullptr;

		FormatChunc format;
		Chunk data;
		char* pBuffer;

	public:		// 関数
		Sound();

		void Initialize();

		// 音データの読み込み
		void LoadFile(const char* filename);

		// 再生
		void Play();

	};
}
/// FPS制御クラス

#pragma once
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

namespace JimaXengine
{
	class FPSManager
	{
	private:
		LARGE_INTEGER timeStart;
		LARGE_INTEGER timeEnd;
		LARGE_INTEGER timeFreq;
		float fps;
		float maxFps;
		DWORD sleepTime;
		int debugCount;

	public:
		void Initialize(const float maxfps);
		void Update();
		float GetFPS() { return fps; }
	};
}
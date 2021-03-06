#include "FPSManager.h"
#include <iostream>

void JimaXengine::FPSManager::Initialize(const float maxfps)
{
	timeStart = {};
	timeEnd = {};
	timeFreq = {};
	sleepTime = {};
	fps = {};
	debugCount = {};
	maxFps = maxfps;

	//CPUから周波数を取得する
	QueryPerformanceFrequency(&timeFreq);

	//初回計算用に取得しておく
	QueryPerformanceCounter(&timeStart);

#ifdef _DEBUG
	printf("fps manager initialized / max fps : %f\n", maxFps);
#endif // _DEBUG

}

void JimaXengine::FPSManager::Update()
{
	if (debugCount == maxFps)
	{
		QueryPerformanceCounter(&timeEnd);
		fps = 1000.0f / (static_cast<float>((timeEnd.QuadPart - timeStart.QuadPart) * 1000.0f / timeFreq.QuadPart) / maxFps);
		debugCount = 0;
		timeStart = timeEnd;
	}
	debugCount++;

	QueryPerformanceCounter(&timeEnd);
	sleepTime = static_cast<DWORD>((1000.0f / maxFps) * debugCount - (timeEnd.QuadPart - timeStart.QuadPart) * 1000.0f / timeFreq.QuadPart);
	//大きく変動がなければSleepTimeは1〜17の間に納まる
	if (sleepTime > 0 && sleepTime < 18)
	{
		timeBeginPeriod(1);
		Sleep(sleepTime);
		timeEndPeriod(1);
	}
	else//大きく変動があった場合
	{
		timeBeginPeriod(1);
		Sleep(1);
		timeEndPeriod(1);
	}
}
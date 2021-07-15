#include "FPSManager.h"
#include <iostream>

void FPSManager::Initialize(const float maxfps)
{
	timeStart = {};
	timeEnd = {};
	timeFreq = {};
	sleepTime = {};
	fps = {};
	debugCount = {};
	maxFps = maxfps;

	//CPU‚©‚çü”g”‚ğæ“¾‚·‚é
	QueryPerformanceFrequency(&timeFreq);

	//‰‰ñŒvZ—p‚Éæ“¾‚µ‚Ä‚¨‚­
	QueryPerformanceCounter(&timeStart);

#ifdef _DEBUG
	printf("fps manager initialized / max fps : %f\n", maxFps);
#endif // _DEBUG

}

void FPSManager::Update()
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
	//‘å‚«‚­•Ï“®‚ª‚È‚¯‚ê‚ÎSleepTime‚Í1`17‚ÌŠÔ‚É”[‚Ü‚é
	if (sleepTime > 0 && sleepTime < 18)
	{
		timeBeginPeriod(1);
		Sleep(sleepTime);
		timeEndPeriod(1);
	}
	else//‘å‚«‚­•Ï“®‚ª‚ ‚Á‚½ê‡
	{
		timeBeginPeriod(1);
		Sleep(1);
		timeEndPeriod(1);
	}
}
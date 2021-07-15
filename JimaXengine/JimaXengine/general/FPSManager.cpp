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

	//CPU������g�����擾����
	QueryPerformanceFrequency(&timeFreq);

	//����v�Z�p�Ɏ擾���Ă���
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
	//�傫���ϓ����Ȃ����SleepTime��1�`17�̊Ԃɔ[�܂�
	if (sleepTime > 0 && sleepTime < 18)
	{
		timeBeginPeriod(1);
		Sleep(sleepTime);
		timeEndPeriod(1);
	}
	else//�傫���ϓ����������ꍇ
	{
		timeBeginPeriod(1);
		Sleep(1);
		timeEndPeriod(1);
	}
}
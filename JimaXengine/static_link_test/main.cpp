#include<Windows.h>
#include "Application.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application::GetInstance().Run();
	return 0;
}
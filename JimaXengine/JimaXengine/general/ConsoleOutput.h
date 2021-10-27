#pragma once
#include <Windows.h>

#define COLOR_RED  0x04
#define COLOR_BLUE 0x01
#define COLOR_YELLOW 0x06
#define COLOR_GREEN 0x02
#define	COLOR_WHITE 0x07

static void ConsoleOutputColor(int a = COLOR_WHITE)
{
	HANDLE hStdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdoutHandle, a);
}
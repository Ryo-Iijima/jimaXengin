#include "General.h"

std::wstring JimaXengine::General::StringToWString(const std::string& name)
{
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, name.c_str(), -1, (wchar_t*)NULL, 0);

	wchar_t* cpUCS2 = new wchar_t[iBufferSize];

	MultiByteToWideChar(CP_ACP, 0, name.c_str(), -1, cpUCS2, iBufferSize);

	std::wstring returnName(cpUCS2, cpUCS2 + iBufferSize - 1);

	delete[] cpUCS2;

	return returnName;
}

unsigned int JimaXengine::General::GetDigit(int& num)
{
	int number = std::abs(num);
	return (unsigned int)std::to_string(number).length();
}

float JimaXengine::General::ConvertToRadians(float& degrees)
{
	return degrees * (MATH_PI / 180.0f);
}

float JimaXengine::General::ConvertToDegrees(float& radians)
{
	return radians * (180.0f / MATH_PI);
}

#pragma once
#include <string>
#include <Windows.h>

namespace JimaXengine
{
	namespace General
	{
		constexpr float MATH_PI = 3.1415f;

		// string����wstring�ւ̕ϊ�
		std::wstring StringToWString(const std::string& name);
		// �������擾
		unsigned int GetDigit(int& num);
		// �x�������W�A��
		float ConvertToRadians(float& degrees);
		// ���W�A�����x��
		float ConvertToDegrees(float& radians);
	};
}
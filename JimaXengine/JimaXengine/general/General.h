#pragma once
#include <string>
#include <Windows.h>

namespace JimaXengine
{
	namespace General
	{
		constexpr float MATH_PI = 3.1415f;

		// stringからwstringへの変換
		std::wstring StringToWString(const std::string& name);
		// 桁数を取得
		unsigned int GetDigit(int& num);
		// 度数→ラジアン
		float ConvertToRadians(float& degrees);
		// ラジアン→度数
		float ConvertToDegrees(float& radians);
	};
}
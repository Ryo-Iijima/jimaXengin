#pragma once
#include <random>

namespace JimaXengine
{
	class Random
	{
	private:
		Random();
	public:
		static float GetRandom(float minValue, float maxValue);
	};
}
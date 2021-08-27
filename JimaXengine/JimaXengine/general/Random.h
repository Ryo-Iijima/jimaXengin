#pragma once
#include <random>

class Random
{
private:
	Random();
public:
	static float GetRandom(float minValue, float maxValue);
};


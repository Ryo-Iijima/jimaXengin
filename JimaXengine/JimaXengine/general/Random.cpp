#include "Random.h"

float Random::GetRandom(float minValue, float maxValue)
{
	std::random_device rnd;
	std::mt19937_64 mt64(rnd());
	std::uniform_real_distribution<float> genRandFloat(minValue, maxValue);
	return genRandFloat(mt64);
}


#include "Runtime/Core/pch.h"
#include "Random.h"

#include <random>

std::default_random_engine LitchiRuntime::Random::__GENERATOR;

int LitchiRuntime::Random::Generate(int p_min, int p_max)
{
	std::uniform_int_distribution<int> distribution(p_min, p_max);
	return distribution(__GENERATOR);
}

float LitchiRuntime::Random::Generate(float p_min, float p_max)
{
	std::uniform_real_distribution<float> distribution(p_min, p_max);
	return distribution(__GENERATOR);
}

bool LitchiRuntime::Random::CheckPercentage(float p_percentage)
{
	std::uniform_real_distribution<float> distribution(0.0f, 100.0f);
	return distribution(__GENERATOR) <= p_percentage;
}

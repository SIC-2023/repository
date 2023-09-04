#include "Random.h"
#include <random>

namespace argent
{
	static std::random_device random_device;
	static std::mt19937 random_engine(random_device());

	float GenerateFRand(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(random_engine);
	}

	int GenerateIRand(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(random_engine);
	}


}
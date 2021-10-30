#ifndef SOURCE_HELPERS_HPP
#define SOURCE_HELPERS_HPP

#include "source/precompiled.h"

int GetRandomInt(int min = 0, int max = 100)
{
	std::random_device dev;
	std::mt19937 generator(dev());
	std::uniform_int_distribution<int> dist(min, max);

	return dist(generator);
}

bool AreEqual(ImVec2 vec1, ImVec2 vec2)
{
	if (vec1.x == vec2.x && vec1.y == vec2.y)
		return true;
	return false;
}

#endif // !SOURCE_HELPERS_HPP

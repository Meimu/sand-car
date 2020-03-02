#pragma once
#include <random>

int _random(int mod)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	return mt() % mod + 1;
}
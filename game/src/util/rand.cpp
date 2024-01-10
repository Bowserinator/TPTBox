// Taken from The Powder Toy
// GPL-3 LICENSED

#include "rand.h"
#include <cstdlib>
#include <ctime>

/* xoroshiro128+ by David Blackman and Sebastiano Vigna */

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t RNG::next() {
	const uint64_t s0 = s[0];
	uint64_t s1 = s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
	s[1] = rotl(s1, 36); // c

	return result;
}

unsigned int RNG::gen() {
	return next() & 0x7FFFFFFF;
}

int RNG::rand() {
	return static_cast<int>(gen());
}

unsigned int RNG::operator()() {
	return next() & 0xFFFFFFFF;
}

int RNG::between(int lower, int upper) {
	unsigned int r = next();
	return static_cast<int>(r % ((unsigned int)(upper) - (unsigned int)(lower) + 1U)) + lower;
}

bool RNG::chance(int numerator, unsigned int denominator) {
	if (numerator < 0)
		return false;
	return next() % denominator < static_cast<unsigned int>(numerator);
}

float RNG::uniform01() {
	return static_cast<float>(next()&0xFFFFFFFF) / (float)0xFFFFFFFF;
}

float RNG::uniform(float lower, float upper) {
	return uniform01() * (upper - lower) + lower;
}

RNG::RNG() {
	s[0] = time(NULL);
	s[1] = 614;
}

void RNG::seed(unsigned int sd) {
	s[0] = sd;
	s[1] = sd;
}

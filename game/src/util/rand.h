// Taken from The Powder Toy
// GPL-3 LICENSED
#ifndef UTIL_RAND_H
#define UTIL_RAND_H

#include <stdint.h>
#include <array>

class RNG {
public:
	using State = std::array<uint64_t, 2>;

private:
	State s;
	uint64_t next();

public:
    // Same as gen()
	unsigned int operator()();

    /**
     * @brief Generate random uint
     * @return unsigned int 
     */
	unsigned int gen();

    /**
     * @brief Generate random int
     * @return int 
     */
    int rand();

    /**
     * @brief Generate a random number between lower
     *        and upper INCLUSIVE
     * @param lower 
     * @param upper 
     * @return int 
     */
	int between(int lower, int upper);

    /**
     * @brief Return true if rand01() < numerator / denom
     *        Note: may not be implemented as such
     * @param numerator
     * @param denominator
     * @return bool
     */
	bool chance(int numerator, unsigned int denominator);

    /**
     * @brief Random float between 0 and 1
     * @return float 
     */
	float uniform01();

    /**
     * @brief Return random float between two values inclusive
     * @param lower 
     * @param upper 
     * @return float 
     */
    float uniform(float lower, float upper);

	RNG();
	void seed(unsigned int sd);

	void state(State ns) {
		s = ns;
	}

	State state() const {
		return s;
	}
};

#endif
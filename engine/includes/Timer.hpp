#pragma once
#include <iostream>
#include <chrono>

/** @brief High-resolution frame timer that returns elapsed time since the last tick. */
class Timer {
	private:
		std::chrono::high_resolution_clock::time_point last;
	public:
		Timer() : last(std::chrono::high_resolution_clock::now()) {};
		/** @brief Returns seconds elapsed since the last call and resets the internal timestamp. */
		float tick();
};

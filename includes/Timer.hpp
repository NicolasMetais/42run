#pragma once
#include <iostream>
#include <chrono>

class Timer {
	private:
		std::chrono::high_resolution_clock::time_point last;
	public:
		Timer() : last(std::chrono::high_resolution_clock::now()) {};
		float tick();
};
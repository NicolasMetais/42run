#include <Timer.hpp>

float Timer::tick() {
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> delta = now - last;
	last = now;
	return delta.count();
};

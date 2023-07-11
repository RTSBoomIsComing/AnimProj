#include "pch.h"
#include "Timer.h"

pa::Timer::Timer()
{
	_lastTimePoint = std::chrono::high_resolution_clock::now();
}

void pa::Timer::update()
{
	const auto	currentTimePoint = std::chrono::high_resolution_clock::now();
	_deltaTime = std::chrono::duration<float>(currentTimePoint - _lastTimePoint).count();

	_lastTimePoint = currentTimePoint;
}

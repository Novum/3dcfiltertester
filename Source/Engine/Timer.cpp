#include "Timer.h"

namespace engine
{
	Timer timer;
	
	Timer::Timer() : paused(true)
	{
		QueryPerformanceFrequency(&frequency);
	}

	float Timer::Tick()
	{
		if(paused) return 0.0f;
		
		LARGE_INTEGER current_count;
		QueryPerformanceCounter(&current_count);

		double frame_time = (double)(current_count.QuadPart-last_count.QuadPart)/(double)frequency.QuadPart;

		last_count = current_count;

		return (float)frame_time;
	}

	void Timer::Start()
	{
		QueryPerformanceCounter(&last_count);	
		paused = false;
	}

	void Timer::Stop()
	{
		paused = true;
	}
}
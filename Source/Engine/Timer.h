#pragma once

#include <windows.h>

namespace engine
{
	class Timer
	{
	public:
		Timer();
		float Tick();
		void Start();
		void Stop();
	private:
		bool paused;
		LARGE_INTEGER frequency;
		LARGE_INTEGER last_count;
	};

	extern Timer timer;
}
#pragma once

#include <chrono>

class Time
{
public:
	Time();
	double dtime() const;
	double uptime() const;
	uint64_t frameCount() const;
	int FPS() const;
	void update();
	~Time();

private:
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point lastTime;
	std::chrono::steady_clock::time_point currentTime;
	std::chrono::steady_clock::duration dTime;
	uint64_t fCount = 0;

	uint64_t fpsFrameCount = 0;
	int fps = 0;
	std::chrono::steady_clock::duration fpsTimer{};
};

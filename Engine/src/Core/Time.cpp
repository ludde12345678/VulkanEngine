#include <Core/Time.h>
#include <chrono>
#include <cstdint>
#include <iostream>
Time::Time() {
	startTime = std::chrono::steady_clock::now();
	lastTime = std::chrono::steady_clock::now();
	currentTime = std::chrono::steady_clock::now();
	dTime = std::chrono::steady_clock::duration::zero();
	fps = 0;
}

double Time::dtime() const
{
	return std::chrono::duration<double>(dTime).count();;

}

double Time::uptime() const
{
	return std::chrono::duration<double>(currentTime - startTime).count();
}
uint64_t Time::frameCount() const {
	return fCount;
}
int Time::FPS() const {
	return fps;
}
void Time::update()
{
	currentTime = std::chrono::steady_clock::now();
	dTime = currentTime - lastTime;
	lastTime = currentTime;

	fCount++;

	fpsFrameCount++;
	fpsTimer += dTime;
	if (fpsTimer >= std::chrono::seconds(1))
	{
		fps = static_cast<int>(fpsFrameCount);
		fpsFrameCount = 0;
		fpsTimer -= std::chrono::seconds(1);
	}
}

Time::~Time() = default;

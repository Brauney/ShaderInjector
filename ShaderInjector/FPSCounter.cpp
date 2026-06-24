#include "FPSCounter.h"

#include <d3d12.h>

namespace FPSCounter
{
	bool gFPSCounterActive = true;
	double gCurrentFPS = 0.0;
	double gCurrentFrameTimeMS = 0.0;
	static LARGE_INTEGER gFPSCounterFrequency{};
	static LARGE_INTEGER gFPSCounterLastSample{};
	static unsigned int gFPSCounterFrameCount = 0;

	void UpdateFPSCounter()
	{
		if (!gFPSCounterActive)
		{
			gCurrentFPS = 0.0;
			gCurrentFrameTimeMS = 0.0;
			gFPSCounterFrameCount = 0;
			gFPSCounterLastSample.QuadPart = 0;
			return;
		}

		if (gFPSCounterFrequency.QuadPart == 0)
			QueryPerformanceFrequency(&gFPSCounterFrequency);

		LARGE_INTEGER now{};
		QueryPerformanceCounter(&now);

		if (gFPSCounterLastSample.QuadPart == 0)
		{
			gFPSCounterLastSample = now;
			gFPSCounterFrameCount = 0;
			return;
		}

		gFPSCounterFrameCount++;
		const double elapsedSeconds = (double)(now.QuadPart - gFPSCounterLastSample.QuadPart) / (double)gFPSCounterFrequency.QuadPart;

		if (elapsedSeconds >= 0.5)
		{
			gCurrentFPS = (double)gFPSCounterFrameCount / elapsedSeconds;
			gCurrentFrameTimeMS = (elapsedSeconds * 1000.0) / (double)gFPSCounterFrameCount;
			gFPSCounterFrameCount = 0;
			gFPSCounterLastSample = now;
		}
	}
}

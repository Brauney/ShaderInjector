#pragma once

namespace FPSCounter
{
	extern bool gFPSCounterActive;
	extern double gCurrentFPS;
	extern double gCurrentFrameTimeMS;

	void UpdateFPSCounter();
}

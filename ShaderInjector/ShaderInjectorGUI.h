#pragma once

#include <string>

#include "ShaderReplacement.h"

namespace ShaderInjectorGUI
{
	using DrawMenuFn = void(*)();

	extern std::string runtimeLogText;

	struct MainWindowContext
	{
		bool* showWindow = nullptr;
		bool injectorEnabled = false;
		bool injectorDeveloperSettings = false;
		bool* fpsCounterActive = nullptr;
		double fps = 0.0;
		double frameTimeMs = 0.0;
		const std::string* runtimeLogText = nullptr;
		DrawMenuFn drawMenu = nullptr;
	};

	void DrawMainWindow(const MainWindowContext& context);

	void UI_ShaderInjectorMenu();
	void UI_ShaderReplacements();
	void UI_StreamPipelines();
	void UI_GraphicsPipelines();
	void UI_D3D12PipelineInfo();
	void UI_AdapterInfo();

	void WriteToRuntimeLog(std::string text);
	void ClearRuntimeLog();
}

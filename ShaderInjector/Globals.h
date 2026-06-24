#pragma once

#include <vector>

namespace Globals
{
	// Handle to our DLL module
	extern HMODULE mainModule;

	// Main game window handle
	extern HWND mainWindow;

	// Key to open/close the ImGui menu (INSERT by default)
	extern int keyOpenShaderInjectorGUI;

	// Key to open/close the ImGui menu (INSERT by default)
	extern int keyToggleShaderInjector;

	static bool gShowShaderInjectorGUI = true;
	static bool gShaderInjectorEnabled = true;

	extern std::vector<uint8_t> nullPixelShaderBlob;
	extern std::vector<uint8_t> markerPixelShaderBlob;
	extern std::vector<uint8_t> markerComputeShaderBlob;
}   

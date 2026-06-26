//Globals.cpp
#include <windows.h>
#include <cstdio>
#include <vector>

//3RD Party
#include "MinHook.h"

//custom
#include "dsound_proxy.h"
#include "HookD3D12.h"
#include "Globals.h"

namespace Globals
{
	// Handle to our DLL module
	HMODULE mainModule = nullptr;

	// Main game window handle
	HWND mainWindow = nullptr;

	// Key to open/close the ImGui menu
	int keyOpenShaderInjectorGUI = VK_INSERT;

	// Key to enable/disable shader injection
	int keyToggleShaderInjector = VK_HOME;

	bool gShowShaderInjectorGUI = true;
	bool gShaderInjectorEnabled = true;

	std::vector<uint8_t> nullPixelShaderBlob;
	std::vector<uint8_t> markerPixelShaderBlob;
	std::vector<uint8_t> markerComputeShaderBlob;
}

//Globals.cpp
#include "PreCompiledHeader.h"
#include <windows.h>
#include "MinHook.h"
#include <cstdio>
#include <vector>

#include "dsound_proxy.h"
#include "HookD3D12.h"
#include "Globals.h"

namespace Globals
{
	// Handle to our DLL module
	extern HMODULE mainModule = nullptr;

	// Main game window handle
	extern HWND mainWindow = nullptr;

	//key to toggle shader injector gui
	extern int keyOpenShaderInjectorGUI = VK_INSERT;

	//key to toggle shader injector
	extern int keyToggleShaderInjector = VK_HOME;

	std::vector<uint8_t> nullPixelShaderBlob;
	std::vector<uint8_t> markerPixelShaderBlob;
	std::vector<uint8_t> markerComputeShaderBlob;
}

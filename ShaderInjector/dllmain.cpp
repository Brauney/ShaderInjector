//dllmain.cpp
#include "PreCompiledHeader.h"
#include <windows.h>
#include "MinHook.h"
#include <cstdio>

//custom
#include "Globals.h"
#include "HookD3D12.h"
#include "Hooks.h"
#include "dsound_proxy.h"
#include "ShaderInjectorIO.h"

//||||||||||||||||||||||||||||||| ON ATTACH |||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||| ON ATTACH |||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||| ON ATTACH |||||||||||||||||||||||||||||||

//thread entry: initialize MinHook and start hook setup
static DWORD WINAPI OnAttachDLL(LPVOID)
{
	//IMPORTANT NOTE 1: The game's D3D12 device already exists before our proxy loads.

	LoadRealDsoundDll();
	Sleep(5000);

	//||||||||||||||||||||||||||||||| INITALIZE IO |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| INITALIZE IO |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| INITALIZE IO |||||||||||||||||||||||||||||||

	ShaderInjectorIO::Initialize();
	ShaderInjectorIO::PurgeLogFile();

	//IMPORTANT NOTE 2: We are able to create and run this thread, so this does execute and work!
	//NOTE 1: keep this comment around for sanity check please!
	//NOTE 2: because this is on a seperate thread, popping a message box will not freeze the application!
	ShaderInjectorIO::WriteToLogFile("dllmain | dsound thread initalized!");

	//||||||||||||||||||||||||||||||| SHADER |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| SHADER |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| SHADER |||||||||||||||||||||||||||||||

	ShaderInjectorIO::WriteToLogFile("dllmain | getting null shader...");

	std::string nullShaderPath = ShaderInjectorIO::GetInternalNullPixelShaderSourceCodeFilePath();
	std::string nullShaderBlobPath = ShaderInjectorIO::GetInternalNullPixelShaderBlobFilePath();

	bool nullShaderCompiled = ShaderInjectorIO::CompileSourceToDXILBlob(
		nullShaderPath, //NOTE: this NEEDS to be an .hlsl source shader text file
		"ps_6_6", //target profile, for rebirth ps_6_6 is what I found in renderdoc
		"main", //name of the function within the shader to execute
		nullShaderBlobPath); //output blob file path

	if (nullShaderCompiled)
	{
		bool nullShaderLoaded = ShaderInjectorIO::LoadDXILBlobFromDisk(
			nullShaderBlobPath, //NOTE: this NEEDS to be a DXIL compiled binary file
			Globals::nullPixelShaderBlob); //array to contain the data in memory

		if (!nullShaderLoaded || Globals::nullPixelShaderBlob.empty())
		{
			Globals::nullPixelShaderBlob.clear();
			ShaderInjectorIO::WriteToLogFile("dllmain | failed to load null pixel shader blob: " + nullShaderBlobPath);
		}
	}
	else
	{
		Globals::nullPixelShaderBlob.clear();
		ShaderInjectorIO::WriteToLogFile("dllmain | failed to compile null pixel shader: " + nullShaderPath);
	}

	ShaderInjectorIO::WriteToLogFile("dllmain | globals::nullPixelShaderBlob size " + std::to_string(Globals::nullPixelShaderBlob.size()));

	bool markerPixelLoaded = ShaderInjectorIO::LoadDXILBlobFromDisk(
		ShaderInjectorIO::GetInternalMarkerPixelShaderBlobFilePath(),
		Globals::markerPixelShaderBlob);

	if (!markerPixelLoaded || Globals::markerPixelShaderBlob.empty())
	{
		Globals::markerPixelShaderBlob.clear();
		ShaderInjectorIO::WriteToLogFile("dllmain | failed to load marker pixel shader blob: " + ShaderInjectorIO::GetInternalMarkerPixelShaderBlobFilePath());
	}

	bool markerComputeLoaded = ShaderInjectorIO::LoadDXILBlobFromDisk(
		ShaderInjectorIO::GetInternalMarkerComputeShaderBlobFilePath(),
		Globals::markerComputeShaderBlob);

	if (!markerComputeLoaded || Globals::markerComputeShaderBlob.empty())
	{
		Globals::markerComputeShaderBlob.clear();
		ShaderInjectorIO::WriteToLogFile("dllmain | failed to load marker compute shader blob: " + ShaderInjectorIO::GetInternalMarkerComputeShaderBlobFilePath());
	}

	ShaderInjectorIO::WriteToLogFile("dllmain | globals::markerPixelShaderBlob size " + std::to_string(Globals::markerPixelShaderBlob.size()));
	ShaderInjectorIO::WriteToLogFile("dllmain | globals::markerComputeShaderBlob size " + std::to_string(Globals::markerComputeShaderBlob.size()));

	//||||||||||||||||||||||||||||||| INITALIZE MINHOOK |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| INITALIZE MINHOOK |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| INITALIZE MINHOOK |||||||||||||||||||||||||||||||

	MH_STATUS status = MH_Initialize();

	if (status != MH_OK)
	{
		char buffer[256];
		sprintf_s(buffer, "dllmain | MH_Initialize failed: %s", MH_StatusToString(status));
		ShaderInjectorIO::WriteToLogFile(buffer);
		return 0;
	}

	//IMPORTANT NOTE 3: We are able to create and initalize minhook, so this does work!
	//NOTE: keep this comment around for sanity check please!
	ShaderInjectorIO::WriteToLogFile("dllmain | minhook initalized!");

	//||||||||||||||||||||||||||||||| D3D12 CHECK |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| D3D12 CHECK |||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||| D3D12 CHECK |||||||||||||||||||||||||||||||

	//NOTE: sanity check, ensure that we have d3d12.dll
	HMODULE d3d12 = GetModuleHandleA("d3d12.dll");

	if (!d3d12)
	{
		MessageBoxA(nullptr, "dllmain | d3d12.dll handle not found!", "Shader Injector", MB_OK);
		return 0;
	}

	//IMPORTANT NOTE 4: We can infact find the d3d12.dll and get a handle on it!
	//NOTE: keep this comment around for sanity check please!
	char buffer[256];
	sprintf_s(buffer, "dllmain | d3d12.dll = %p", d3d12);
	ShaderInjectorIO::WriteToLogFile(buffer);

	HookD3D12::InstallD3D12CreateDeviceHook(d3d12);

	//ref - https://github.com/Sh0ckFR/Universal-Dear-ImGui-Hook/blob/master/dllmain.cpp
	ShaderInjectorIO::WriteToLogFile("dllmain | Attempting DX12 initialization...");
	Hooks::Initalize();

	return 0;
}

//||||||||||||||||||||||||||||||| DLL MAIN |||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||| DLL MAIN |||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||| DLL MAIN |||||||||||||||||||||||||||||||

//hModule: handle to DLL module
//reason: reason for calling function
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
	//perform actions based on the reason for calling.
	switch (reason)
	{
		//||||||||||||||||||||||||||||||| DLL ATTACHMENT |||||||||||||||||||||||||||||||
		//||||||||||||||||||||||||||||||| DLL ATTACHMENT |||||||||||||||||||||||||||||||
		//||||||||||||||||||||||||||||||| DLL ATTACHMENT |||||||||||||||||||||||||||||||
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hModule);

			Globals::mainModule = hModule;

			//FIX: surrounded most of this with brackets, as we get a wierd scoping issue involving the thread
			//Error	C2360: initialization of 'hookThread' is skipped by 'case' label
			{
				//it's important that we create a seperate thread for hooking, otherwise we block the application from loading at all!
				HANDLE hookThread = CreateThread(
					nullptr,
					0,
					OnAttachDLL,
					nullptr,
					0,
					nullptr);

				if (hookThread)
				{
					CloseHandle(hookThread);

					//NOTE 1: keep this comment around just in case we hit headaches later, sanity check to verify if we are even creating a hook thread
					//NOTE 2: popping a message box here will freeze the application!
					//MessageBoxA(nullptr, "dllmain | Created hook thread!", "Shader Injector", MB_OK);
				}
				else
				{
					//NOTE 1: keep this comment around just in case we hit headaches later, sanity check to verify when we aren't able to create a hook thread
					//NOTE 2: popping a message box here will freeze the application!
					MessageBoxA(nullptr, "dllmain | Failed to create hook thread!", "Shader Injector", MB_OK);
				}
	

				//NOTE 1: keep this comment around just in case we hit headaches later, sanity check to verify if the dll is even getting attached
				//NOTE 2: popping a message box here will freeze the application!
				//MessageBoxA(nullptr, "dllmain | dsound attached!", "Shader Injector", MB_OK);
			}

			break;

		//||||||||||||||||||||||||||||||| DLL DETATCHMENT |||||||||||||||||||||||||||||||
		//||||||||||||||||||||||||||||||| DLL DETATCHMENT |||||||||||||||||||||||||||||||
		//||||||||||||||||||||||||||||||| DLL DETATCHMENT |||||||||||||||||||||||||||||||
		case DLL_PROCESS_DETACH:

			FreeRealDsoundDll();

			//cleanup minhook
			MH_DisableHook(MH_ALL_HOOKS);
			MH_RemoveHook(MH_ALL_HOOKS);
			MH_Uninitialize();

			break;
	}

	return TRUE; //successful DLL_PROCESS_ATTACH.
}

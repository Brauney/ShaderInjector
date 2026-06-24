#include "PreCompiledHeader.h"
#include "dsound_proxy.h"

#include <cstdio>
#include <mmsystem.h>
#include <dsound.h>

HMODULE g_realDsoundDll = nullptr;
static INIT_ONCE g_dsoundInitOnce = INIT_ONCE_STATIC_INIT;

using pfnDirectSoundCaptureCreate = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUNDCAPTURE*, LPUNKNOWN);
using pfnDirectSoundCaptureCreate8 = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUNDCAPTURE8*, LPUNKNOWN);
using pfnDirectSoundCaptureEnumerateA = HRESULT(WINAPI*)(LPDSENUMCALLBACKA, LPVOID);
using pfnDirectSoundCaptureEnumerateW = HRESULT(WINAPI*)(LPDSENUMCALLBACKW, LPVOID);
using pfnDirectSoundCreate = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUND*, LPUNKNOWN);
using pfnDirectSoundCreate8 = HRESULT(WINAPI*)(LPCGUID, LPDIRECTSOUND8*, LPUNKNOWN);
using pfnDirectSoundEnumerateA = HRESULT(WINAPI*)(LPDSENUMCALLBACKA, LPVOID);
using pfnDirectSoundEnumerateW = HRESULT(WINAPI*)(LPDSENUMCALLBACKW, LPVOID);
using pfnDirectSoundFullDuplexCreate = HRESULT(WINAPI*)(
	LPCGUID,
	LPCGUID,
	LPCDSCBUFFERDESC,
	LPCDSBUFFERDESC,
	HWND,
	DWORD,
	LPDIRECTSOUNDFULLDUPLEX*,
	LPDIRECTSOUNDCAPTUREBUFFER8*,
	LPDIRECTSOUNDBUFFER8*,
	LPUNKNOWN);
using pfnDllCanUnloadNow = HRESULT(WINAPI*)();
using pfnDllGetClassObject = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);
using pfnGetDeviceID = HRESULT(WINAPI*)(LPCGUID, LPGUID);

static pfnDirectSoundCaptureCreate p_DirectSoundCaptureCreate = nullptr;
static pfnDirectSoundCaptureCreate8 p_DirectSoundCaptureCreate8 = nullptr;
static pfnDirectSoundCaptureEnumerateA p_DirectSoundCaptureEnumerateA = nullptr;
static pfnDirectSoundCaptureEnumerateW p_DirectSoundCaptureEnumerateW = nullptr;
static pfnDirectSoundCreate p_DirectSoundCreate = nullptr;
static pfnDirectSoundCreate8 p_DirectSoundCreate8 = nullptr;
static pfnDirectSoundEnumerateA p_DirectSoundEnumerateA = nullptr;
static pfnDirectSoundEnumerateW p_DirectSoundEnumerateW = nullptr;
static pfnDirectSoundFullDuplexCreate p_DirectSoundFullDuplexCreate = nullptr;
static pfnDllCanUnloadNow p_DllCanUnloadNow = nullptr;
static pfnDllGetClassObject p_DllGetClassObject = nullptr;
static pfnGetDeviceID p_GetDeviceID = nullptr;

template <typename T>
static T LoadExport(const char* exportName)
{
	return reinterpret_cast<T>(GetProcAddress(g_realDsoundDll, exportName));
}

static BOOL CALLBACK LoadRealDsoundDllOnce(PINIT_ONCE, PVOID, PVOID*)
{
	char systemDir[MAX_PATH];
	GetSystemDirectoryA(systemDir, MAX_PATH);

	char dllPath[MAX_PATH];
	sprintf_s(dllPath, "%s\\dsound.dll", systemDir);

	g_realDsoundDll = LoadLibraryA(dllPath);

	if (g_realDsoundDll)
	{
		p_DirectSoundCaptureCreate = LoadExport<pfnDirectSoundCaptureCreate>("DirectSoundCaptureCreate");
		p_DirectSoundCaptureCreate8 = LoadExport<pfnDirectSoundCaptureCreate8>("DirectSoundCaptureCreate8");
		p_DirectSoundCaptureEnumerateA = LoadExport<pfnDirectSoundCaptureEnumerateA>("DirectSoundCaptureEnumerateA");
		p_DirectSoundCaptureEnumerateW = LoadExport<pfnDirectSoundCaptureEnumerateW>("DirectSoundCaptureEnumerateW");
		p_DirectSoundCreate = LoadExport<pfnDirectSoundCreate>("DirectSoundCreate");
		p_DirectSoundCreate8 = LoadExport<pfnDirectSoundCreate8>("DirectSoundCreate8");
		p_DirectSoundEnumerateA = LoadExport<pfnDirectSoundEnumerateA>("DirectSoundEnumerateA");
		p_DirectSoundEnumerateW = LoadExport<pfnDirectSoundEnumerateW>("DirectSoundEnumerateW");
		p_DirectSoundFullDuplexCreate = LoadExport<pfnDirectSoundFullDuplexCreate>("DirectSoundFullDuplexCreate");
		p_DllCanUnloadNow = LoadExport<pfnDllCanUnloadNow>("DllCanUnloadNow");
		p_DllGetClassObject = LoadExport<pfnDllGetClassObject>("DllGetClassObject");
		p_GetDeviceID = LoadExport<pfnGetDeviceID>("GetDeviceID");
	}

	return TRUE;
}

bool EnsureRealDsoundDllLoaded()
{
	InitOnceExecuteOnce(&g_dsoundInitOnce, LoadRealDsoundDllOnce, nullptr, nullptr);
	return g_realDsoundDll != nullptr;
}

bool LoadRealDsoundDll()
{
	return EnsureRealDsoundDllLoaded();
}

void FreeRealDsoundDll()
{
	if (g_realDsoundDll)
	{
		FreeLibrary(g_realDsoundDll);
		g_realDsoundDll = nullptr;
	}
}

extern "C"
{
	HRESULT WINAPI DirectSoundCaptureCreate(
		LPCGUID pcGuidDevice,
		LPDIRECTSOUNDCAPTURE* ppDSC,
		LPUNKNOWN pUnkOuter)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundCaptureCreate ? p_DirectSoundCaptureCreate(pcGuidDevice, ppDSC, pUnkOuter) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundCaptureCreate8(
		LPCGUID pcGuidDevice,
		LPDIRECTSOUNDCAPTURE8* ppDSC8,
		LPUNKNOWN pUnkOuter)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundCaptureCreate8 ? p_DirectSoundCaptureCreate8(pcGuidDevice, ppDSC8, pUnkOuter) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundCaptureEnumerateA(
		LPDSENUMCALLBACKA pDSEnumCallback,
		LPVOID pContext)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundCaptureEnumerateA ? p_DirectSoundCaptureEnumerateA(pDSEnumCallback, pContext) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundCaptureEnumerateW(
		LPDSENUMCALLBACKW pDSEnumCallback,
		LPVOID pContext)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundCaptureEnumerateW ? p_DirectSoundCaptureEnumerateW(pDSEnumCallback, pContext) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundCreate(
		LPCGUID pcGuidDevice,
		LPDIRECTSOUND* ppDS,
		LPUNKNOWN pUnkOuter)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundCreate ? p_DirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundCreate8(
		LPCGUID pcGuidDevice,
		LPDIRECTSOUND8* ppDS8,
		LPUNKNOWN pUnkOuter)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundCreate8 ? p_DirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundEnumerateA(
		LPDSENUMCALLBACKA pDSEnumCallback,
		LPVOID pContext)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundEnumerateA ? p_DirectSoundEnumerateA(pDSEnumCallback, pContext) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundEnumerateW(
		LPDSENUMCALLBACKW pDSEnumCallback,
		LPVOID pContext)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundEnumerateW ? p_DirectSoundEnumerateW(pDSEnumCallback, pContext) : E_FAIL;
	}

	HRESULT WINAPI DirectSoundFullDuplexCreate(
		LPCGUID pcGuidCaptureDevice,
		LPCGUID pcGuidRenderDevice,
		LPCDSCBUFFERDESC pcDSCBufferDesc,
		LPCDSBUFFERDESC pcDSBufferDesc,
		HWND hWnd,
		DWORD dwLevel,
		LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
		LPDIRECTSOUNDCAPTUREBUFFER8* ppDSCBuffer8,
		LPDIRECTSOUNDBUFFER8* ppDSBuffer8,
		LPUNKNOWN pUnkOuter)
	{
		EnsureRealDsoundDllLoaded();
		return p_DirectSoundFullDuplexCreate
			? p_DirectSoundFullDuplexCreate(
				pcGuidCaptureDevice,
				pcGuidRenderDevice,
				pcDSCBufferDesc,
				pcDSBufferDesc,
				hWnd,
				dwLevel,
				ppDSFD,
				ppDSCBuffer8,
				ppDSBuffer8,
				pUnkOuter)
			: E_FAIL;
	}

	HRESULT WINAPI DllCanUnloadNow()
	{
		EnsureRealDsoundDllLoaded();
		return p_DllCanUnloadNow ? p_DllCanUnloadNow() : S_FALSE;
	}

	HRESULT WINAPI DllGetClassObject(
		REFCLSID rclsid,
		REFIID riid,
		LPVOID* ppv)
	{
		EnsureRealDsoundDllLoaded();
		return p_DllGetClassObject ? p_DllGetClassObject(rclsid, riid, ppv) : CLASS_E_CLASSNOTAVAILABLE;
	}

	HRESULT WINAPI GetDeviceID(
		LPCGUID pGuidSrc,
		LPGUID pGuidDest)
	{
		EnsureRealDsoundDllLoaded();
		return p_GetDeviceID ? p_GetDeviceID(pGuidSrc, pGuidDest) : E_FAIL;
	}
}

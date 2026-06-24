#pragma once
#include "PreCompiledHeader.h"
#include <Windows.h>

extern HMODULE g_realDsoundDll;

bool LoadRealDsoundDll();
void FreeRealDsoundDll();
bool EnsureRealDsoundDllLoaded();

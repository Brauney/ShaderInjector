#pragma once

//custom
#include "HookD3D12.h"

namespace HookD3D12
{
	bool CompileShaderTarget(int replacementIndex);
	bool ReloadShaderTarget(int replacementIndex);
	bool SaveShaderTarget(int replacementIndex);
	bool DeleteShaderTarget(int replacementIndex);

	void RefreshLoadedShaderTargets();

	void SyncShaderTargetNameBuffer();
}

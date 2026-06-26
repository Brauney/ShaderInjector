#pragma once

//custom
#include "HookD3D12.h"

namespace HookD3D12
{
	bool CompileShaderReplacement(int replacementIndex);
	bool ReloadShaderReplacement(int replacementIndex);
	bool SaveShaderReplacement(int replacementIndex);
	bool DeleteShaderReplacement(int replacementIndex);

	void RefreshLoadedShaderReplacements();

	void SyncShaderReplacementNameBuffer();
}

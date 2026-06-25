#pragma once

#include "HookD3D12.h"

namespace HookD3D12
{
	void RefreshLoadedShaderReplacements();
	void SyncShaderReplacementNameBuffer();
	bool SaveShaderReplacement(int index);
	bool CompileShaderReplacement(int index);
	bool ReloadShaderReplacement(int index);
	bool DeleteShaderReplacement(int index);
}

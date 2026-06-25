#pragma once

#include <string>
#include <vector>

#include "ShaderReplacement.h"

namespace DatabaseShaderSources
{
	std::string ShaderSourceSubdirectoryForType(ShaderReplacement::ShaderType shaderType);
	std::string ResolveShaderSourcePath(ShaderReplacement::ShaderType shaderType, const std::string& shaderSourceName);
	void SyncReplacementShaderSourcePath(ShaderReplacement::ShaderReplacementDisk& replacement);
	void RefreshShaderSources(ShaderReplacement::ShaderType shaderType);
	ShaderReplacement::ShaderType GetShaderSourceListType();
	const std::vector<std::string>& GetShaderSourceFiles();
}

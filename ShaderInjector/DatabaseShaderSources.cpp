#include "PreCompiledHeader.h"

#include "DatabaseShaderSources.h"
#include "ShaderInjectorIO.h"

#include <algorithm>

namespace DatabaseShaderSources
{
	namespace
	{
		ShaderReplacement::ShaderType gShaderSourceListType = ShaderReplacement::Unknown;
		std::vector<std::string> gShaderSourceFiles;
	}

	std::string ShaderSourceSubdirectoryForType(ShaderReplacement::ShaderType shaderType)
	{
		return ShaderReplacement::ShaderTypeToString(shaderType) + "s";
	}

	std::string ResolveShaderSourcePath(ShaderReplacement::ShaderType shaderType, const std::string& shaderSourceName)
	{
		if (shaderSourceName.empty())
			return "";

		return ShaderInjectorIO::GetShaderSourcesDirectory(ShaderSourceSubdirectoryForType(shaderType)) + "\\" + shaderSourceName;
	}

	void SyncReplacementShaderSourcePath(ShaderReplacement::ShaderReplacementDisk& replacement)
	{
		if (replacement.shaderSourceName.empty() && !replacement.shaderSourcePath.empty())
			replacement.shaderSourceName = ShaderInjectorIO::FileNameFromPath(replacement.shaderSourcePath);

		if (!replacement.shaderSourceName.empty())
			replacement.shaderSourcePath = ResolveShaderSourcePath(replacement.shaderType, replacement.shaderSourceName);
	}

	void RefreshShaderSources(ShaderReplacement::ShaderType shaderType)
	{
		gShaderSourceFiles.clear();
		gShaderSourceListType = shaderType;

		const std::string directory = ShaderInjectorIO::GetShaderSourcesDirectory(ShaderSourceSubdirectoryForType(shaderType));
		ShaderInjectorIO::DirectoryCreate(ShaderInjectorIO::GetShaderSourcesDirectory());
		ShaderInjectorIO::DirectoryCreate(directory);

		ShaderInjectorIO::CollectFilesByExtension(directory, ShaderInjectorIO::extensionHLSL, gShaderSourceFiles, false, false);
		std::sort(gShaderSourceFiles.begin(), gShaderSourceFiles.end());
	}

	ShaderReplacement::ShaderType GetShaderSourceListType()
	{
		return gShaderSourceListType;
	}

	const std::vector<std::string>& GetShaderSourceFiles()
	{
		return gShaderSourceFiles;
	}
}

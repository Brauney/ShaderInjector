//ShaderReplacement.cpp
#include "ShaderReplacement.h"

#include <windows.h>
#include <fstream>
#include <vector>

//custom
#include "ShaderInjectorIO.h"
#include "StringHelper.h"

namespace ShaderReplacement
{
	void MakePathFieldPortable(std::string& filePath)
	{
		if (!filePath.empty())
			filePath = ShaderInjectorIO::FileNameFromPath(filePath);
	}

	void ResolvePathField(std::string& filePath, const std::string& replacementDirectory)
	{
		if (filePath.empty() || replacementDirectory.empty())
			return;

		filePath = replacementDirectory + "\\" + ShaderInjectorIO::FileNameFromPath(filePath);
	}

	std::string ShaderSourceSubdirectoryForType(ShaderReplacement::ShaderType shaderType)
	{
		return StringHelper::ShaderTypeToString(shaderType) + "s";
	}

	std::vector<std::string*> PathFields(ShaderReplacement::ShaderReplacementDisk& replacement)
	{
		// These fields are stored beside the replacement JSON. Keeping them in one list
		// makes save/load path normalization consistent for current and future metadata.
		std::vector<std::string*> pathFields =
		{
			&replacement.originalShaderBlobPath,
			&replacement.modifiedShaderBlobPath,
			&replacement.jsonPath,
			&replacement.pipelineCachedBlobPath,
			&replacement.pipelineStreamBlobPath,
			&replacement.pipelineStreamMetadataPath,
			&replacement.rootSignatureBlobPath,
			&replacement.vertexShaderBlobPath,
			&replacement.pixelShaderBlobPath,
			&replacement.computeShaderBlobPath,
			&replacement.geometryShaderBlobPath,
			&replacement.hullShaderBlobPath,
			&replacement.domainShaderBlobPath,
		};

		for (ShaderReplacement::ShaderPipelineTemplateDisk& pipelineTemplate : replacement.pipelineTemplates)
		{
			pathFields.push_back(&pipelineTemplate.pipelineCachedBlobPath);
			pathFields.push_back(&pipelineTemplate.pipelineStreamBlobPath);
			pathFields.push_back(&pipelineTemplate.pipelineStreamMetadataPath);
			pathFields.push_back(&pipelineTemplate.rootSignatureBlobPath);
			pathFields.push_back(&pipelineTemplate.vertexShaderBlobPath);
			pathFields.push_back(&pipelineTemplate.pixelShaderBlobPath);
			pathFields.push_back(&pipelineTemplate.computeShaderBlobPath);
			pathFields.push_back(&pipelineTemplate.geometryShaderBlobPath);
			pathFields.push_back(&pipelineTemplate.hullShaderBlobPath);
			pathFields.push_back(&pipelineTemplate.domainShaderBlobPath);
		}

		return pathFields;
	}

	void MakeReplacementPortableForDisk(ShaderReplacement::ShaderReplacementDisk& replacement)
	{
		// JSON should remain portable between machines, so persist filenames only.
		if (replacement.shaderSourceName.empty() && !replacement.shaderSourcePath.empty())
			replacement.shaderSourceName = ShaderInjectorIO::FileNameFromPath(replacement.shaderSourcePath);

		if (!replacement.shaderSourceName.empty())
			replacement.shaderSourcePath = replacement.shaderSourceName;

		for (std::string* filePath : PathFields(replacement))
			MakePathFieldPortable(*filePath);

		replacement.replacementDirectory = ".";
	}

	void ResolveReplacementPathsFromJsonLocation(ShaderReplacement::ShaderReplacementDisk& replacement, const std::string& jsonPath)
	{
		const std::string replacementDirectory = ShaderInjectorIO::DirectoryFromPath(jsonPath);
		replacement.replacementDirectory = replacementDirectory;

		for (std::string* filePath : PathFields(replacement))
			ResolvePathField(*filePath, replacementDirectory);

		const std::string legacySourcePath = replacement.shaderSourcePath.empty()
			? ""
			: replacementDirectory + "\\" + ShaderInjectorIO::FileNameFromPath(replacement.shaderSourcePath);

		if (replacement.shaderSourceName.empty() && !replacement.shaderSourcePath.empty())
			replacement.shaderSourceName = ShaderInjectorIO::FileNameFromPath(replacement.shaderSourcePath);

		if (!replacement.shaderSourceName.empty())
		{
			// Shader source files now live under the centralized ShaderSources tree.
			// This migration keeps older per-replacement source files usable.
			const std::string centralSourceDirectory = ShaderInjectorIO::GetShaderSourcesDirectory(ShaderSourceSubdirectoryForType(replacement.shaderType));
			const std::string centralSourcePath =
				centralSourceDirectory +
				"\\" +
				replacement.shaderSourceName;

			ShaderInjectorIO::DirectoryCreate(ShaderInjectorIO::GetShaderSourcesDirectory());
			ShaderInjectorIO::DirectoryCreate(centralSourceDirectory);

			if (!legacySourcePath.empty() && !ShaderInjectorIO::FileExists(centralSourcePath) && ShaderInjectorIO::FileExists(legacySourcePath))
				CopyFileA(legacySourcePath.c_str(), centralSourcePath.c_str(), TRUE);

			replacement.shaderSourcePath = centralSourcePath;
		}

		replacement.jsonPath = jsonPath;
	}

	bool IsShaderReplacementJsonFilename(const char* filename)
	{
		if (!filename)
			return false;

		const std::string name = filename;
		return name == "ShaderReplacement.json";
	}

	bool WriteShaderReplacementJson(const ShaderReplacement::ShaderReplacementDisk& replacement)
	{
		std::ofstream jsonFile(replacement.jsonPath, std::ios::out | std::ios::trunc);

		if (!jsonFile.is_open())
			return false;

		ShaderReplacement::ShaderReplacementDisk portableReplacement = replacement;
		MakeReplacementPortableForDisk(portableReplacement);

		nlohmann::ordered_json json = portableReplacement;
		json["pipelineTemplates"] = portableReplacement.pipelineTemplates;
		jsonFile << json.dump(4);

		return !jsonFile.fail();
	}

	bool LoadShaderReplacementJson(const std::string& path, ShaderReplacement::ShaderReplacementDisk& outReplacement)
	{
		try
		{
			std::ifstream jsonFile(path);

			if (!jsonFile.is_open())
				return false;

			nlohmann::ordered_json json{};
			jsonFile >> json;
			outReplacement = json.get<ShaderReplacement::ShaderReplacementDisk>();
			if (json.contains("pipelineTemplates") && json["pipelineTemplates"].is_array())
				outReplacement.pipelineTemplates = json["pipelineTemplates"].get<std::vector<ShaderReplacement::ShaderPipelineTemplateDisk>>();

			ResolveReplacementPathsFromJsonLocation(outReplacement, path);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool WritePipelineStreamMetadataJson(const std::string& path, const ShaderReplacement::ShaderPipelineStreamMetadataDisk& metadata)
	{
		std::ofstream jsonFile(path, std::ios::out | std::ios::trunc);

		if (!jsonFile.is_open())
			return false;

		nlohmann::ordered_json json = metadata;
		jsonFile << json.dump(4);

		return !jsonFile.fail();
	}

	bool LoadPipelineStreamMetadataJson(const std::string& path, ShaderReplacement::ShaderPipelineStreamMetadataDisk& outMetadata)
	{
		try
		{
			std::ifstream jsonFile(path);

			if (!jsonFile.is_open())
				return false;

			nlohmann::ordered_json json{};
			jsonFile >> json;
			outMetadata = json.get<ShaderReplacement::ShaderPipelineStreamMetadataDisk>();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void CollectShaderReplacementJsonFiles(const std::string& directory, std::vector<std::string>& outJsonFiles)
	{
		std::vector<std::string> jsonFiles;
		ShaderInjectorIO::CollectFilesByExtension(directory, ShaderInjectorIO::extensionJSON, jsonFiles, true);

		for (const std::string& jsonFile : jsonFiles)
		{
			if (IsShaderReplacementJsonFilename(ShaderInjectorIO::FileNameFromPath(jsonFile).c_str()))
				outJsonFiles.push_back(jsonFile);
		}
	}
}

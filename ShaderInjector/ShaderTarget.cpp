//ShaderTarget.cpp
#include "ShaderTarget.h"

#include <fstream>
#include <vector>

//custom
#include "ShaderInjectorIO.h"

namespace ShaderTarget
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

		filePath = ShaderInjectorIO::JoinPath(replacementDirectory, ShaderInjectorIO::FileNameFromPath(filePath));
	}

	std::vector<std::string*> PathFields(ShaderTarget::ShaderTargetDisk& replacement)
	{
		// These fields are stored beside the replacement JSON. Keeping them in one list
		// makes save/load path normalization consistent for current and future metadata.
		std::vector<std::string*> pathFields =
		{
			&replacement.originalShaderBlobPath,
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

		for (ShaderTarget::ShaderPipelineTemplateDisk& pipelineTemplate : replacement.pipelineTemplates)
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

	void MakeReplacementPortableForDisk(ShaderTarget::ShaderTargetDisk& replacement)
	{
		for (std::string* filePath : PathFields(replacement))
			MakePathFieldPortable(*filePath);

		replacement.replacementDirectory = ".";
	}

	void ResolveReplacementPathsFromJsonLocation(ShaderTarget::ShaderTargetDisk& replacement, const std::string& jsonPath)
	{
		const std::string replacementDirectory = ShaderInjectorIO::DirectoryFromPath(jsonPath);
		replacement.replacementDirectory = replacementDirectory;

		for (std::string* filePath : PathFields(replacement))
			ResolvePathField(*filePath, replacementDirectory);

		replacement.jsonPath = jsonPath;
	}

	bool IsShaderTargetJsonFilename(const char* filename)
	{
		if (!filename)
			return false;

		const std::string name = filename;
		return name == "ShaderTarget.json";
	}

	bool WriteShaderTargetJson(const ShaderTarget::ShaderTargetDisk& replacement)
	{
		std::ofstream jsonFile(replacement.jsonPath, std::ios::out | std::ios::trunc);

		if (!jsonFile.is_open())
			return false;

		ShaderTarget::ShaderTargetDisk portableReplacement = replacement;
		MakeReplacementPortableForDisk(portableReplacement);

		nlohmann::ordered_json json = portableReplacement;
		json["shaderBytecodeHashAliases"] = portableReplacement.shaderBytecodeHashAliases;
		json["originalShaderAnalysis"] = portableReplacement.originalShaderAnalysis;
		json["pipelineTemplates"] = portableReplacement.pipelineTemplates;
		jsonFile << json.dump(4);

		return !jsonFile.fail();
	}

	bool LoadShaderTargetJson(const std::string& path, ShaderTarget::ShaderTargetDisk& outReplacement)
	{
		try
		{
			std::ifstream jsonFile(path);

			if (!jsonFile.is_open())
				return false;

			nlohmann::ordered_json json{};
			jsonFile >> json;
			outReplacement = json.get<ShaderTarget::ShaderTargetDisk>();
			if (json.contains("shaderBytecodeHashAliases") && json["shaderBytecodeHashAliases"].is_array())
				outReplacement.shaderBytecodeHashAliases = json["shaderBytecodeHashAliases"].get<std::vector<std::string>>();
			if (json.contains("originalShaderAnalysis") && json["originalShaderAnalysis"].is_object())
				outReplacement.originalShaderAnalysis = json["originalShaderAnalysis"].get<ShaderAnalysis::ShaderAnalysisDisk>();
			if (json.contains("pipelineTemplates") && json["pipelineTemplates"].is_array())
				outReplacement.pipelineTemplates = json["pipelineTemplates"].get<std::vector<ShaderTarget::ShaderPipelineTemplateDisk>>();

			ResolveReplacementPathsFromJsonLocation(outReplacement, path);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool WritePipelineStreamMetadataJson(const std::string& path, const ShaderTarget::ShaderPipelineStreamMetadataDisk& metadata)
	{
		std::ofstream jsonFile(path, std::ios::out | std::ios::trunc);

		if (!jsonFile.is_open())
			return false;

		nlohmann::ordered_json json = metadata;
		jsonFile << json.dump(4);

		return !jsonFile.fail();
	}

	bool LoadPipelineStreamMetadataJson(const std::string& path, ShaderTarget::ShaderPipelineStreamMetadataDisk& outMetadata)
	{
		try
		{
			std::ifstream jsonFile(path);

			if (!jsonFile.is_open())
				return false;

			nlohmann::ordered_json json{};
			jsonFile >> json;
			outMetadata = json.get<ShaderTarget::ShaderPipelineStreamMetadataDisk>();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void CollectShaderTargetJsonFiles(const std::string& directory, std::vector<std::string>& outJsonFiles)
	{
		std::vector<std::string> jsonFiles;
		ShaderInjectorIO::CollectFilesByExtension(directory, ShaderInjectorIO::extensionJSON, jsonFiles, true);

		for (const std::string& jsonFile : jsonFiles)
		{
			if (IsShaderTargetJsonFilename(ShaderInjectorIO::FileNameFromPath(jsonFile).c_str()))
				outJsonFiles.push_back(jsonFile);
		}
	}
}

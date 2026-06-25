#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <d3d12.h>

#include "HookD3D12.h"
#include "ShaderReplacement.h"

namespace HookD3D12
{
	bool LoadPersistedShaderBlob(const std::string& path, std::vector<uint8_t>& bytecode, uint64_t& hash, SIZE_T& size);
	bool LoadPersistedStreamTemplateFromReplacement(const ShaderReplacement::ShaderReplacementDisk& replacement, PipelineStateInfo& outPipeline);
	void BackfillReplacementPortableMetadataFromSidecars(ShaderReplacement::ShaderReplacementDisk& replacement);
	uint64_t StreamShaderHashForType(const PipelineStateInfo& pipeline, ShaderReplacement::ShaderType shaderType);
	bool StreamPipelineHasShaderHash(const PipelineStateInfo& pipeline, ShaderReplacement::ShaderType shaderType, uint64_t shaderHash);
	void FillPipelineTemplateCommonState(ShaderReplacement::ShaderPipelineTemplateDisk& pipelineTemplate, const PipelineStateInfo& pipeline);
	ShaderReplacement::ShaderReplacementDisk ReplacementWithPipelineTemplate(const ShaderReplacement::ShaderReplacementDisk& replacement, const ShaderReplacement::ShaderPipelineTemplateDisk& pipelineTemplate);
	SIZE_T CountMatchingBytes(const std::vector<uint8_t>& lhs, const std::vector<uint8_t>& rhs);
	bool WriteStreamPipelineTemplateVariant(ShaderReplacement::ShaderReplacementDisk& replacement, const PipelineStateInfo& pipeline, int pipelineIndex, int templateIndex, bool& ok);
	void WriteMatchingStreamPipelineTemplateVariants(ShaderReplacement::ShaderReplacementDisk& replacement, ShaderReplacement::ShaderType shaderType, uint64_t shaderHash, bool& ok);
	bool SelectPersistedPipelineTemplateForUncaptured(
		const ShaderReplacement::ShaderReplacementDisk& replacement,
		const UncapturedPipelineStateInfo& uncaptured,
		ShaderReplacement::ShaderReplacementDisk& outTemplateReplacement,
		std::string& outTemplateName,
		SIZE_T& outMatchingBytes);
}

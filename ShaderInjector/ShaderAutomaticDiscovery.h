#pragma once

#include <cstdint>
#include <vector>

#include "ShaderTarget.h"

namespace HookD3D12
{
	struct GraphicsPipelineInfo;
	struct PipelineStateInfo;
}

namespace ShaderAutomaticDiscovery
{
	void ProcessCapturedGraphicsPipeline(HookD3D12::GraphicsPipelineInfo& pipeline);
	void ProcessCapturedStreamPipeline(HookD3D12::PipelineStateInfo& pipeline);

	bool ProcessCapturedShader(
		HookD3D12::GraphicsPipelineInfo& pipeline,
		int pipelineIndex,
		ShaderTarget::ShaderType shaderType,
		uint64_t shaderHash,
		const std::vector<uint8_t>& shaderBytecode);
	bool ProcessCapturedShader(
		HookD3D12::PipelineStateInfo& pipeline,
		int pipelineIndex,
		ShaderTarget::ShaderType shaderType,
		uint64_t shaderHash,
		const std::vector<uint8_t>& shaderBytecode);
}

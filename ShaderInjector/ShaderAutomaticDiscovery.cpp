#include "ShaderAutomaticDiscovery.h"

#include <mutex>
#include <vector>

#include "DatabaseModifiedShaders.h"
#include "DatabaseShaderTargets.h"
#include "Hash.h"
#include "HookD3D12.h"
#include "ShaderDiscovery.h"
#include "ShaderInjectorGUI.h"

namespace ShaderAutomaticDiscovery
{
	namespace
	{
		std::mutex gAutomaticDiscoveryMutex;

		bool TargetContainsHash(const ShaderTarget::ShaderTargetDisk& target, uint64_t shaderHash)
		{
			if (Hash::ParseHashText(target.originalShaderBytecodeHash) == shaderHash)
				return true;

			for (const std::string& aliasHash : target.shaderBytecodeHashAliases)
			{
				if (Hash::ParseHashText(aliasHash) == shaderHash)
					return true;
			}
			return false;
		}

		template<typename PipelineType>
		bool CreateTargetForMatch(
			const char* sourceList,
			PipelineType& pipeline,
			int pipelineIndex,
			ShaderTarget::ShaderType shaderType,
			uint64_t shaderHash,
			const std::vector<uint8_t>& shaderBytecode)
		{
			if (shaderHash == 0 || shaderBytecode.empty())
				return false;

			std::lock_guard<std::mutex> discoveryLock(gAutomaticDiscoveryMutex);
			DatabaseModifiedShaders::EnsureModifiedShadersLoaded();
			const std::vector<ModifiedShader::PackageDisk>& modifiedShaders =
				DatabaseModifiedShaders::GetModifiedShaders();
			const int modifiedShaderIndex = ShaderDiscovery::DiscoverEnabledModifiedShader(
				shaderHash,
				shaderType,
				shaderBytecode,
				modifiedShaders);
			if (modifiedShaderIndex < 0 || modifiedShaderIndex >= static_cast<int>(modifiedShaders.size()))
				return false;

			const std::string modifiedShaderId = modifiedShaders[modifiedShaderIndex].id;
			if (!HookD3D12::gLoadedShaderTargetsOnce)
				HookD3D12::RefreshLoadedShaderTargets();

			for (int targetIndex = 0; targetIndex < static_cast<int>(HookD3D12::gLoadedShaderTargets.size()); ++targetIndex)
			{
				const ShaderTarget::ShaderTargetDisk& existingTarget = HookD3D12::gLoadedShaderTargets[targetIndex];
				if (existingTarget.shaderType != shaderType || !TargetContainsHash(existingTarget, shaderHash))
				{
					continue;
				}

				if (existingTarget.modifiedShaderId != modifiedShaderId)
				{
					ShaderInjectorGUI::WriteToRuntimeLogError(
						"ShaderAutomaticDiscovery: shader " + Hash::FormatHash(shaderHash) +
						" already belongs to target " + existingTarget.name +
						" using a different ModifiedShader");
					return false;
				}

				if (!existingTarget.enabled)
					return false;

				const ModifiedShader::PackageDisk* existingPackage =
					DatabaseModifiedShaders::FindModifiedShaderById(modifiedShaderId);
				if (existingPackage && existingPackage->compiledBlob.empty())
				{
					if (!DatabaseModifiedShaders::CompileModifiedShader(modifiedShaderId))
					{
						ShaderInjectorGUI::WriteToRuntimeLogError(
							"ShaderAutomaticDiscovery: failed to compile " + modifiedShaderId);
						return false;
					}

					if (!HookD3D12::ReloadShaderTarget(targetIndex))
						return false;
				}

				HookD3D12::MarkShaderTargetApplyDirty();
				return true;
			}

			const ModifiedShader::PackageDisk* selectedPackage =
				DatabaseModifiedShaders::FindModifiedShaderById(modifiedShaderId);
			if (!selectedPackage)
				return false;

			if (selectedPackage->compiledBlob.empty() &&
				!DatabaseModifiedShaders::CompileModifiedShader(modifiedShaderId))
			{
				ShaderInjectorGUI::WriteToRuntimeLogError(
					"ShaderAutomaticDiscovery: failed to compile " + modifiedShaderId);
				return false;
			}

			if (!HookD3D12::CreateShaderTargetForPipeline(
				sourceList,
				pipelineIndex,
				shaderType,
				shaderHash,
				shaderBytecode.size(),
				shaderBytecode.data(),
				pipeline,
				modifiedShaderId,
				false))
			{
				return false;
			}

			HookD3D12::RefreshLoadedShaderTargets();
			HookD3D12::MarkShaderTargetApplyDirty();
			ShaderInjectorGUI::WriteToRuntimeLogSuccess(
				"ShaderAutomaticDiscovery: created ShaderTarget for " + Hash::FormatHash(shaderHash) +
				" using " + modifiedShaderId);
			return true;
		}
	}

	void ProcessCapturedGraphicsPipeline(HookD3D12::GraphicsPipelineInfo& pipeline)
	{
		CreateTargetForMatch("Graphics", pipeline, -1, ShaderTarget::VertexShader, pipeline.vsHash, pipeline.vsBytecode);
		CreateTargetForMatch("Graphics", pipeline, -1, ShaderTarget::PixelShader, pipeline.psHash, pipeline.psBytecode);
		CreateTargetForMatch("Graphics", pipeline, -1, ShaderTarget::GeometryShader, pipeline.gsHash, pipeline.gsBytecode);
		CreateTargetForMatch("Graphics", pipeline, -1, ShaderTarget::HullShader, pipeline.hsHash, pipeline.hsBytecode);
		CreateTargetForMatch("Graphics", pipeline, -1, ShaderTarget::DomainShader, pipeline.dsHash, pipeline.dsBytecode);
	}

	void ProcessCapturedStreamPipeline(HookD3D12::PipelineStateInfo& pipeline)
	{
		CreateTargetForMatch("Stream", pipeline, -1, ShaderTarget::VertexShader, pipeline.vsHash, pipeline.vsBytecode);
		CreateTargetForMatch("Stream", pipeline, -1, ShaderTarget::PixelShader, pipeline.psHash, pipeline.psBytecode);
		CreateTargetForMatch("Stream", pipeline, -1, ShaderTarget::ComputeShader, pipeline.csHash, pipeline.csBytecode);
		CreateTargetForMatch("Stream", pipeline, -1, ShaderTarget::GeometryShader, pipeline.gsHash, pipeline.gsBytecode);
		CreateTargetForMatch("Stream", pipeline, -1, ShaderTarget::HullShader, pipeline.hsHash, pipeline.hsBytecode);
		CreateTargetForMatch("Stream", pipeline, -1, ShaderTarget::DomainShader, pipeline.dsHash, pipeline.dsBytecode);
	}

	bool ProcessCapturedShader(
		HookD3D12::GraphicsPipelineInfo& pipeline,
		int pipelineIndex,
		ShaderTarget::ShaderType shaderType,
		uint64_t shaderHash,
		const std::vector<uint8_t>& shaderBytecode)
	{
		return CreateTargetForMatch("Graphics", pipeline, pipelineIndex, shaderType, shaderHash, shaderBytecode);
	}

	bool ProcessCapturedShader(
		HookD3D12::PipelineStateInfo& pipeline,
		int pipelineIndex,
		ShaderTarget::ShaderType shaderType,
		uint64_t shaderHash,
		const std::vector<uint8_t>& shaderBytecode)
	{
		return CreateTargetForMatch("Stream", pipeline, pipelineIndex, shaderType, shaderHash, shaderBytecode);
	}
}
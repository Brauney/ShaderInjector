#include "PreCompiledHeader.h"

#include "DatabaseGraphicsPSOs.h"
#include "Hash.h"
#include "HookD3D12PipelineRegistry.h"

#include <mutex>
#include <vector>

namespace HookD3D12
{
	std::vector<GraphicsPipelineInfo> gGraphicsPipelines;

	namespace
	{
		std::vector<ComputePipelineInfo> gComputePipelines;
	}

	void CaptureGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc, ID3D12PipelineState* pipelineState)
	{
		if (!desc || !pipelineState)
			return;

		GraphicsPipelineInfo info{};
		info.pipelineState = pipelineState;

		if (desc->VS.pShaderBytecode && desc->VS.BytecodeLength)
		{
			info.vsHash = Hash::HashMemory(desc->VS.pShaderBytecode, desc->VS.BytecodeLength);
			info.vsSize = desc->VS.BytecodeLength;
			info.vsBytecode.assign((const uint8_t*)desc->VS.pShaderBytecode, (const uint8_t*)desc->VS.pShaderBytecode + desc->VS.BytecodeLength);
		}

		if (desc->PS.pShaderBytecode && desc->PS.BytecodeLength)
		{
			info.psHash = Hash::HashMemory(desc->PS.pShaderBytecode, desc->PS.BytecodeLength);
			info.psSize = desc->PS.BytecodeLength;
			info.psBytecode.assign((const uint8_t*)desc->PS.pShaderBytecode, (const uint8_t*)desc->PS.pShaderBytecode + desc->PS.BytecodeLength);
		}

		if (desc->GS.pShaderBytecode && desc->GS.BytecodeLength)
		{
			info.gsHash = Hash::HashMemory(desc->GS.pShaderBytecode, desc->GS.BytecodeLength);
			info.gsSize = desc->GS.BytecodeLength;
			info.gsBytecode.assign((const uint8_t*)desc->GS.pShaderBytecode, (const uint8_t*)desc->GS.pShaderBytecode + desc->GS.BytecodeLength);
		}

		if (desc->HS.pShaderBytecode && desc->HS.BytecodeLength)
		{
			info.hsHash = Hash::HashMemory(desc->HS.pShaderBytecode, desc->HS.BytecodeLength);
			info.hsSize = desc->HS.BytecodeLength;
			info.hsBytecode.assign((const uint8_t*)desc->HS.pShaderBytecode, (const uint8_t*)desc->HS.pShaderBytecode + desc->HS.BytecodeLength);
		}

		if (desc->DS.pShaderBytecode && desc->DS.BytecodeLength)
		{
			info.dsHash = Hash::HashMemory(desc->DS.pShaderBytecode, desc->DS.BytecodeLength);
			info.dsSize = desc->DS.BytecodeLength;
			info.dsBytecode.assign((const uint8_t*)desc->DS.pShaderBytecode, (const uint8_t*)desc->DS.pShaderBytecode + desc->DS.BytecodeLength);
		}

		info.originalDesc = *desc;
		info.originalDesc.VS = { info.vsBytecode.empty() ? nullptr : info.vsBytecode.data(), info.vsBytecode.size() };
		info.originalDesc.PS = { info.psBytecode.empty() ? nullptr : info.psBytecode.data(), info.psBytecode.size() };
		info.originalDesc.GS = { info.gsBytecode.empty() ? nullptr : info.gsBytecode.data(), info.gsBytecode.size() };
		info.originalDesc.HS = { info.hsBytecode.empty() ? nullptr : info.hsBytecode.data(), info.hsBytecode.size() };
		info.originalDesc.DS = { info.dsBytecode.empty() ? nullptr : info.dsBytecode.data(), info.dsBytecode.size() };
		info.originalDesc.pRootSignature = desc->pRootSignature;

		if (info.originalDesc.pRootSignature)
			info.originalDesc.pRootSignature->AddRef();

		if (desc->InputLayout.pInputElementDescs && desc->InputLayout.NumElements > 0)
		{
			info.inputElements.assign(desc->InputLayout.pInputElementDescs, desc->InputLayout.pInputElementDescs + desc->InputLayout.NumElements);
			info.originalDesc.InputLayout.pInputElementDescs = info.inputElements.data();
			info.originalDesc.InputLayout.NumElements = (UINT)info.inputElements.size();
		}
		else
		{
			info.originalDesc.InputLayout.pInputElementDescs = nullptr;
			info.originalDesc.InputLayout.NumElements = 0;
		}

		if (desc->StreamOutput.pSODeclaration && desc->StreamOutput.NumEntries > 0)
		{
			info.soDeclarations.assign(desc->StreamOutput.pSODeclaration, desc->StreamOutput.pSODeclaration + desc->StreamOutput.NumEntries);
			info.originalDesc.StreamOutput.pSODeclaration = info.soDeclarations.data();
		}
		else
		{
			info.originalDesc.StreamOutput.pSODeclaration = nullptr;
			info.originalDesc.StreamOutput.NumEntries = 0;
		}

		if (desc->StreamOutput.pBufferStrides && desc->StreamOutput.NumStrides > 0)
		{
			info.soStrides.assign(desc->StreamOutput.pBufferStrides, desc->StreamOutput.pBufferStrides + desc->StreamOutput.NumStrides);
			info.originalDesc.StreamOutput.pBufferStrides = info.soStrides.data();
		}
		else
		{
			info.originalDesc.StreamOutput.pBufferStrides = nullptr;
			info.originalDesc.StreamOutput.NumStrides = 0;
		}

		info.originalDesc.CachedPSO.pCachedBlob = nullptr;
		info.originalDesc.CachedPSO.CachedBlobSizeInBytes = 0;

		std::lock_guard<std::mutex> lock(gPipelineMutex);
		RegisterKnownPipelineStateLocked(pipelineState);
		gGraphicsPipelines.push_back(info);
		MarkShaderReplacementApplyDirty();
	}

	void CaptureComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* desc, ID3D12PipelineState* pipelineState, bool registerKnownPipeline)
	{
		if (!desc || !pipelineState)
			return;

		ComputePipelineInfo info{};
		info.pipelineState = pipelineState;

		if (desc->CS.pShaderBytecode && desc->CS.BytecodeLength)
		{
			info.csHash = Hash::HashMemory(desc->CS.pShaderBytecode, desc->CS.BytecodeLength);
			info.csSize = desc->CS.BytecodeLength;
		}

		std::lock_guard<std::mutex> lock(gPipelineMutex);

		if (registerKnownPipeline)
			RegisterKnownPipelineStateLocked(info.pipelineState);

		gComputePipelines.push_back(info);
	}
}

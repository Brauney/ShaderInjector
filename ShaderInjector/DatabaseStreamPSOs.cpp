#include "PreCompiledHeader.h"

#include "DatabaseStreamPSOs.h"
#include "HookD3D12PipelineRegistry.h"
#include "HookD3D12PipelineUtils.h"

#include <mutex>
#include <vector>

namespace HookD3D12
{
	std::vector<PipelineStateInfo> gPipelineStates;

	void CapturePipelineStateStream(const D3D12_PIPELINE_STATE_STREAM_DESC* desc, ID3D12PipelineState* pipelineState)
	{
		if (!desc || !desc->pPipelineStateSubobjectStream || desc->SizeInBytes == 0 || !pipelineState)
			return;

		PipelineStateInfo info{};
		info.pipelineState = pipelineState;

		const uint8_t* streamStart = (const uint8_t*)desc->pPipelineStateSubobjectStream;
		info.streamBlob.assign(streamStart, streamStart + desc->SizeInBytes);

		ParsePipelineStream(desc, info);

		std::lock_guard<std::mutex> lock(gPipelineMutex);
		RegisterKnownPipelineStateLocked(pipelineState);
		gPipelineStates.push_back(info);
		RebindPipelineStateInfoPointerFields(gPipelineStates.back());
		MarkShaderReplacementApplyDirty();
	}
}

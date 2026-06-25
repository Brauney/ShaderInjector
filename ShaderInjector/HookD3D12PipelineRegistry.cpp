#include "PreCompiledHeader.h"

#include "HookD3D12PipelineRegistry.h"

#include <unordered_set>

namespace HookD3D12
{
	namespace
	{
		std::unordered_set<ID3D12PipelineState*> gKnownPipelineStates;
		std::unordered_set<ID3D12PipelineState*> gUntrackedBoundPipelineStates;
	}

	void RegisterKnownPipelineStateLocked(ID3D12PipelineState* pipelineStateObject)
	{
		if (pipelineStateObject)
			gKnownPipelineStates.insert(pipelineStateObject);
	}

	void UnregisterKnownPipelineStateLocked(ID3D12PipelineState* pipelineStateObject)
	{
		if (pipelineStateObject)
			gKnownPipelineStates.erase(pipelineStateObject);
	}

	bool IsKnownPipelineStateLocked(ID3D12PipelineState* pipelineStateObject)
	{
		return !pipelineStateObject || gKnownPipelineStates.find(pipelineStateObject) != gKnownPipelineStates.end();
	}

	bool MarkUntrackedBoundPipelineStateLocked(ID3D12PipelineState* pipelineStateObject)
	{
		return pipelineStateObject && gUntrackedBoundPipelineStates.insert(pipelineStateObject).second;
	}
}

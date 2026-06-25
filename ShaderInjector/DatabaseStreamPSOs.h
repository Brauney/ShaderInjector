#pragma once

#include "HookD3D12.h"

namespace HookD3D12
{
	void CapturePipelineStateStream(const D3D12_PIPELINE_STATE_STREAM_DESC* desc, ID3D12PipelineState* pipelineState);
}

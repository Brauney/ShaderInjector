#pragma once

#include "ShaderAnalysis.h"

namespace ShaderAnalyzer
{
	bool Analyze(const void* bytecode, size_t bytecodeLength, ShaderAnalysis::ShaderAnalysisDisk& outAnalysis);
}

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "ModifiedShader.h"
#include "ShaderAnalysis.h"
#include "ShaderDiscovery.h"
#include "ShaderTarget.h"

namespace CrossVersionDiscoveryTest
{
	struct ModifiedShaderMatchResult
	{
		bool accepted = false;
		int packageIndex = -1;
		double bestScore = 0.0;
		double secondBestScore = 0.0;
		bool matchedByHash = false;
	};

	struct ReplacementMatchResult
	{
		bool accepted = false;
		int replacementIndex = -1;
		bool passedLengthGate = false;
		bool hasStrictCandidateIdentity = false;
		bool matchedByExactIdentity = false;
		bool matchedByFuzzySimilarity = false;
		double bestFuzzyScore = 0.0;
		double secondBestFuzzyScore = 0.0;
	};

	bool HasPlausibleReplacementByteLength(size_t candidateLength, const ShaderTarget::ShaderTargetDisk& replacement);

	bool HasStrictCrossVersionIdentity(const ShaderAnalysis::ShaderAnalysisDisk& analysis);

	bool WouldEnqueueByByteLength(ShaderTarget::ShaderType shaderType, size_t byteLength, const std::vector<size_t>& knownTargetLengths);

	ReplacementMatchResult EvaluateReplacementDiscovery(
		uint64_t shaderHash,
		ShaderTarget::ShaderType shaderType,
		size_t shaderBytecodeLength,
		const ShaderAnalysis::ShaderAnalysisDisk& candidateAnalysis,
		const std::vector<ShaderTarget::ShaderTargetDisk>& replacements);

	ModifiedShaderMatchResult EvaluateModifiedShaderDiscovery(
		uint64_t shaderHash,
		ShaderTarget::ShaderType shaderType,
		size_t shaderBytecodeLength,
		const ShaderAnalysis::ShaderAnalysisDisk& candidateAnalysis,
		const std::vector<ModifiedShader::PackageDisk>& modifiedShaders);

	ShaderTarget::ShaderTargetDisk MakeReplacementFromPackageTarget(
		const ModifiedShader::TargetDisk& target,
		ShaderTarget::ShaderType shaderType,
		const std::string& name);
}

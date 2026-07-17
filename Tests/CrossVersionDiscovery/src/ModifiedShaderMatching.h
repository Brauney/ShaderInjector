#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ShaderAnalysis.h"

namespace ModifiedShader
{
	struct TargetDisk;
	struct PackageDisk;
}

namespace ModifiedShaderMatching
{
	bool TargetMatchesShader(
		const ModifiedShader::TargetDisk& target,
		uint64_t shaderHash,
		const ShaderAnalysis::ShaderAnalysisDisk& analysis);

	double CalculateTargetSimilarityScore(const ModifiedShader::TargetDisk& left, const ModifiedShader::TargetDisk& right);

	bool PackageMatchesShader(
		const ModifiedShader::PackageDisk& package,
		uint64_t shaderHash,
		const ShaderAnalysis::ShaderAnalysisDisk& analysis);
}

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "JsonHelper.h"

namespace ShaderAnalysis
{
	struct ContainerPartDisk
	{
		std::string fourCC;
		uint32_t kind = 0;
		uint64_t size = 0;
		std::string contentHash;

		double CalculateSimilarityScore(const ContainerPartDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<ContainerPartDisk>& left, const std::vector<ContainerPartDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ContainerPartDisk, fourCC, kind, size, contentHash)
	};

	struct SignatureParameterDisk
	{
		std::string semanticName;
		uint32_t semanticIndex = 0;
		uint32_t registerIndex = 0;
		uint32_t systemValueType = 0;
		uint32_t componentType = 0;
		uint32_t mask = 0;
		uint32_t readWriteMask = 0;
		uint32_t stream = 0;
		uint32_t minimumPrecision = 0;

		double CalculateSimilarityScore(const SignatureParameterDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<SignatureParameterDisk>& left, const std::vector<SignatureParameterDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			SignatureParameterDisk,
			semanticName,
			semanticIndex,
			registerIndex,
			systemValueType,
			componentType,
			mask,
			readWriteMask,
			stream,
			minimumPrecision)
	};

	struct ResourceBindingDisk
	{
		std::string name;
		uint32_t type = 0;
		uint32_t bindPoint = 0;
		uint32_t bindCount = 0;
		uint32_t flags = 0;
		uint32_t returnType = 0;
		uint32_t dimension = 0;
		uint32_t sampleCountOrStride = 0;
		uint32_t registerSpace = 0;
		uint32_t rangeId = 0;

		double CalculateSimilarityScore(const ResourceBindingDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<ResourceBindingDisk>& left, const std::vector<ResourceBindingDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			ResourceBindingDisk,
			name,
			type,
			bindPoint,
			bindCount,
			flags,
			returnType,
			dimension,
			sampleCountOrStride,
			registerSpace,
			rangeId)
	};

	struct TypeLayoutDisk
	{
		std::string name;
		uint32_t variableClass = 0;
		uint32_t variableType = 0;
		uint32_t rows = 0;
		uint32_t columns = 0;
		uint32_t elements = 0;
		uint32_t offset = 0;
		std::vector<TypeLayoutDisk> members;

		double CalculateSimilarityScore(const TypeLayoutDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<TypeLayoutDisk>& left, const std::vector<TypeLayoutDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			TypeLayoutDisk,
			name,
			variableClass,
			variableType,
			rows,
			columns,
			elements,
			offset,
			members)
	};

	struct ConstantBufferVariableDisk
	{
		std::string name;
		uint32_t startOffset = 0;
		uint32_t size = 0;
		uint32_t flags = 0;
		uint32_t startTexture = 0;
		uint32_t textureCount = 0;
		uint32_t startSampler = 0;
		uint32_t samplerCount = 0;
		TypeLayoutDisk typeLayout;

		double CalculateSimilarityScore(const ConstantBufferVariableDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<ConstantBufferVariableDisk>& left, const std::vector<ConstantBufferVariableDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			ConstantBufferVariableDisk,
			name,
			startOffset,
			size,
			flags,
			startTexture,
			textureCount,
			startSampler,
			samplerCount,
			typeLayout)
	};

	struct ConstantBufferDisk
	{
		std::string name;
		uint32_t type = 0;
		uint32_t size = 0;
		uint32_t flags = 0;
		std::vector<ConstantBufferVariableDisk> variables;

		double CalculateSimilarityScore(const ConstantBufferDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<ConstantBufferDisk>& left, const std::vector<ConstantBufferDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ConstantBufferDisk, name, type, size, flags, variables)
	};

	struct InstructionStatisticsDisk
	{
		uint32_t instructionCount = 0;
		uint32_t temporaryRegisterCount = 0;
		uint32_t temporaryArrayCount = 0;
		uint32_t constantDefinitionCount = 0;
		uint32_t declarationCount = 0;
		uint32_t textureNormalInstructionCount = 0;
		uint32_t textureLoadInstructionCount = 0;
		uint32_t textureComparisonInstructionCount = 0;
		uint32_t textureBiasInstructionCount = 0;
		uint32_t textureGradientInstructionCount = 0;
		uint32_t floatInstructionCount = 0;
		uint32_t signedIntegerInstructionCount = 0;
		uint32_t unsignedIntegerInstructionCount = 0;
		uint32_t staticFlowControlCount = 0;
		uint32_t dynamicFlowControlCount = 0;
		uint32_t macroInstructionCount = 0;
		uint32_t arrayInstructionCount = 0;
		uint32_t cutInstructionCount = 0;
		uint32_t emitInstructionCount = 0;
		uint32_t barrierInstructionCount = 0;
		uint32_t interlockedInstructionCount = 0;
		uint32_t textureStoreInstructionCount = 0;

		double CalculateSimilarityScore(const InstructionStatisticsDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<InstructionStatisticsDisk>& left, const std::vector<InstructionStatisticsDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			InstructionStatisticsDisk,
			instructionCount,
			temporaryRegisterCount,
			temporaryArrayCount,
			constantDefinitionCount,
			declarationCount,
			textureNormalInstructionCount,
			textureLoadInstructionCount,
			textureComparisonInstructionCount,
			textureBiasInstructionCount,
			textureGradientInstructionCount,
			floatInstructionCount,
			signedIntegerInstructionCount,
			unsignedIntegerInstructionCount,
			staticFlowControlCount,
			dynamicFlowControlCount,
			macroInstructionCount,
			arrayInstructionCount,
			cutInstructionCount,
			emitInstructionCount,
			barrierInstructionCount,
			interlockedInstructionCount,
			textureStoreInstructionCount)
	};

	struct ExecutionPropertiesDisk
	{
		uint32_t geometryOutputTopology = 0;
		uint32_t geometryMaximumOutputVertexCount = 0;
		uint32_t inputPrimitive = 0;
		uint32_t geometryInstanceCount = 0;
		uint32_t controlPointCount = 0;
		uint32_t hullOutputPrimitive = 0;
		uint32_t hullPartitioning = 0;
		uint32_t tessellatorDomain = 0;
		uint32_t threadGroupSizeX = 0;
		uint32_t threadGroupSizeY = 0;
		uint32_t threadGroupSizeZ = 0;
		uint32_t threadGroupTotalSize = 0;
		uint32_t minimumFeatureLevel = 0;
		uint64_t requiresFlags = 0;
		bool sampleFrequencyShader = false;

		double CalculateSimilarityScore(const ExecutionPropertiesDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<ExecutionPropertiesDisk>& left, const std::vector<ExecutionPropertiesDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			ExecutionPropertiesDisk,
			geometryOutputTopology,
			geometryMaximumOutputVertexCount,
			inputPrimitive,
			geometryInstanceCount,
			controlPointCount,
			hullOutputPrimitive,
			hullPartitioning,
			tessellatorDomain,
			threadGroupSizeX,
			threadGroupSizeY,
			threadGroupSizeZ,
			threadGroupTotalSize,
			minimumFeatureLevel,
			requiresFlags,
			sampleFrequencyShader)
	};

	struct ShaderAnalysisDisk
	{
		uint32_t analysisVersion = 1;
		bool succeeded = false;
		std::string error;
		std::string creator;
		uint32_t shaderVersionToken = 0;
		uint32_t shaderStage = 0;
		std::string shaderStageName;
		uint32_t shaderModelMajor = 0;
		uint32_t shaderModelMinor = 0;
		std::string shaderModelProfile;
		uint32_t compilationFlags = 0;
		std::string containerSignatureHash;
		std::string interfaceSignatureHash;
		std::string resourceSignatureHash;
		std::string constantBufferSignatureHash;
		std::string instructionStatisticsHash;
		std::string executionSignatureHash;
		std::string portableReflectionIdentityHash;
		std::string entryFunctionName;
		uint32_t normalizedInstructionCount = 0;
		uint32_t uniqueNormalizedInstructionCount = 0;
		std::string semanticInstructionSetHash;
		std::string crossVersionIdentityHash;
		std::string reflectionSignatureHash;
		std::vector<ContainerPartDisk> containerParts;
		std::vector<SignatureParameterDisk> inputParameters;
		std::vector<SignatureParameterDisk> outputParameters;
		std::vector<SignatureParameterDisk> patchConstantParameters;
		std::vector<ResourceBindingDisk> resourceBindings;
		std::vector<ConstantBufferDisk> constantBuffers;
		InstructionStatisticsDisk instructionStatistics;
		ExecutionPropertiesDisk executionProperties;

		double CalculateSimilarityScore(const ShaderAnalysisDisk& other) const;
		static double CalculateSimilarityScore(const std::vector<ShaderAnalysisDisk>& left, const std::vector<ShaderAnalysisDisk>& right);

		NLOHMANN_ORDERED_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			ShaderAnalysisDisk,
			analysisVersion,
			succeeded,
			error,
			creator,
			shaderVersionToken,
			shaderStage,
			shaderStageName,
			shaderModelMajor,
			shaderModelMinor,
			shaderModelProfile,
			compilationFlags,
			containerSignatureHash,
			interfaceSignatureHash,
			resourceSignatureHash,
			constantBufferSignatureHash,
			instructionStatisticsHash,
			executionSignatureHash,
			portableReflectionIdentityHash,
			entryFunctionName,
			normalizedInstructionCount,
			uniqueNormalizedInstructionCount,
			semanticInstructionSetHash,
			crossVersionIdentityHash,
			reflectionSignatureHash,
			containerParts,
			inputParameters,
			outputParameters,
			patchConstantParameters,
			resourceBindings,
			constantBuffers,
			instructionStatistics,
			executionProperties)
	};

}

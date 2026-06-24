#include "ShaderTemplates.h"

namespace ShaderTemplates
{
	std::string ReplacementTemplateSourceForType(ShaderReplacement::ShaderType shaderType)
	{
		switch(shaderType)
		{
			case ShaderReplacement::PixelShader: return templatePixelShaderSourceCode;
			case ShaderReplacement::ComputeShader: return templateComputeShaderSourceCode;
			default:
				return
				"// Shader replacement template. Fill in the entry point and compile target from the JSON.\n"
				"// The original shader bytecode is dumped beside this file for disassembly/reference.\n";
		}
	}
}

#include "StringHelper.h"

#include <string>

//custom 
#include "ShaderReplacement.h"

namespace StringHelper
{
	std::string PointerToString(const void* ptr)
	{
		char buffer[32]{};
		sprintf_s(buffer, "%p", ptr);
		return buffer;
	}

	std::string ShaderTypeToString(ShaderReplacement::ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderReplacement::VertexShader: return "VertexShader";
		case ShaderReplacement::HullShader: return "HullShader";
		case ShaderReplacement::DomainShader: return "DomainShader";
		case ShaderReplacement::GeometryShader: return "GeometryShader";
		case ShaderReplacement::PixelShader: return "PixelShader";
		case ShaderReplacement::ComputeShader: return "ComputeShader";
		default: return "Unknown";
		}
	}

	std::string ShaderProfileForType(ShaderReplacement::ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderReplacement::VertexShader: return "vs_6_6";
		case ShaderReplacement::HullShader: return "hs_6_6";
		case ShaderReplacement::DomainShader: return "ds_6_6";
		case ShaderReplacement::GeometryShader: return "gs_6_6";
		case ShaderReplacement::PixelShader: return "ps_6_6";
		case ShaderReplacement::ComputeShader: return "cs_6_6";
		default: return "";
		}
	}
}
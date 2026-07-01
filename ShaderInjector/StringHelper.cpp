#include "StringHelper.h"

#include <string>

//custom 
#include "ShaderTarget.h"

namespace StringHelper
{
	std::string PointerToString(const void* ptr)
	{
		char buffer[32]{};
		sprintf_s(buffer, "%p", ptr);
		return buffer;
	}

	std::string ShaderTypeToString(ShaderTarget::ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderTarget::VertexShader: return "VertexShader";
		case ShaderTarget::HullShader: return "HullShader";
		case ShaderTarget::DomainShader: return "DomainShader";
		case ShaderTarget::GeometryShader: return "GeometryShader";
		case ShaderTarget::PixelShader: return "PixelShader";
		case ShaderTarget::ComputeShader: return "ComputeShader";
		default: return "Unknown";
		}
	}

	std::string ShaderProfileForType(ShaderTarget::ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderTarget::VertexShader: return "vs_6_6";
		case ShaderTarget::HullShader: return "hs_6_6";
		case ShaderTarget::DomainShader: return "ds_6_6";
		case ShaderTarget::GeometryShader: return "gs_6_6";
		case ShaderTarget::PixelShader: return "ps_6_6";
		case ShaderTarget::ComputeShader: return "cs_6_6";
		default: return "";
		}
	}
}
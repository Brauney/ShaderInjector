#pragma once

#include <string>

//custom 
#include "ShaderTarget.h"

namespace StringHelper
{
	std::string PointerToString(const void* ptr);

	std::string ShaderTypeToString(ShaderTarget::ShaderType shaderType);
	std::string ShaderProfileForType(ShaderTarget::ShaderType shaderType);
}
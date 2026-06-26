#pragma once

#include <string>

//custom 
#include "ShaderReplacement.h"

namespace StringHelper
{
	std::string PointerToString(const void* ptr);

	std::string ShaderTypeToString(ShaderReplacement::ShaderType shaderType);
	std::string ShaderProfileForType(ShaderReplacement::ShaderType shaderType);
}
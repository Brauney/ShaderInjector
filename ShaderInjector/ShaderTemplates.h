#pragma once

#include "ShaderInjectorIO.h"
#include "ShaderReplacement.h"

namespace ShaderTemplates
{
	//NOTE: These are source code shader templates that the shader injector will auto-generate for various tasks.
	//Now normally I would think it'd be wise to actually have these already serialized to the disk rather than holding them in memory.
	//But... I do not trust users... so for sanity sake these will remain in memory.
	//That way if anything goes wrong, we can just rebuild some of these shaders and it should still work...

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| PIXEL SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| PIXEL SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| PIXEL SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||

	//NOTE TO SELF: watch those spaces!
	static const char* internalMarkerPixelShaderSourceCode = R"(
float4 main() : SV_Target0
{
	return float4(0, 0, 1, 1); //blue for marking
}
)";

	//NOTE TO SELF: watch those spaces!
	static const char* internalNullPixelShaderSourceCode = R"(
float4 main() : SV_Target0
{
	return float4(1, 0, 0, 1); //red for error
}
)";

	//NOTE TO SELF: watch those spaces!
	static const char* templatePixelShaderSourceCode = R"(
//Hello! This is an auto-generated pixel shader template from ShaderInjector!
//This is a pixel-shader, this is a GPU program that will execute for every pixel that it is drawn on a screen.
//Make sure that the function name remains the same, otherwise dxc.exe won't find the entry point!

float4 main() : SV_Target0
{
	float4 finalColor = float4(0, 1, 0, 1); //green for success!

	//have fun!

	return finalColor;
}
)";

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| COMPUTE SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| COMPUTE SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| COMPUTE SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||

	//NOTE TO SELF: watch those spaces!
	static const char* internalMarkerComputeShaderSourceCode = R"(
struct InputStruct 
{
	uint3 DispatchThreadID : SV_DispatchThreadID;
};

[numthreads(8, 8, 1)]
void main(in InputStruct IN)
{

}
)";

	//NOTE TO SELF: watch those spaces!
	static const char* templateComputeShaderSourceCode = R"(
//Hello! This is an auto-generated shader template from ShaderInjector!
//This is a pixel-shader, this is a GPU program that will execute for every pixel that it is drawn on a screen.
//Make sure that the function name remains the same, otherwise dxc.exe won't find the entry point.

struct InputStruct 
{
	uint3 DispatchThreadID : SV_DispatchThreadID;
};

//NOTE: default thread count for template, I would ADVISE you check the compiled/dxil for actual counts!
[numthreads(8, 8, 1)]
void main(in InputStruct IN)
{
	//NOTE: compute shaders normally write to an RW______ Type (i.e. RWTexture2D<float4> output)
	//you will either need to check renderdoc, or the dxil for those resources that are passed to the compute shader.
	//otherwise you won't be able to output anything!

	//have fun!
}
)";

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| METHODS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| METHODS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| METHODS |||||||||||||||||||||||||||||||||||||||||||||||||||||

	std::string ReplacementTemplateSourceForType(ShaderReplacement::ShaderType shaderType);
}

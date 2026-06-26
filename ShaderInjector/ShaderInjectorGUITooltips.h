#pragma once

namespace ShaderInjectorGUI
{
	static const char* noteDeveloperSettingsText =
"Shaders/PSOs can only be caught during shader compilation, which can be achieved by deleting the game shader cache. "
"During shader compilation this is where we catch new shaders being built. "
"From there we can then find a shader, and create a 'shader replacement' template for modification later inside the 'Shader Replacements' menu up above. "
"YOU ONLY NEED TO DO THIS ONCE if you are trying to setup a replacement template for a shader you want to modify on first launch after deleting the game shader cache. "
"(Replacement shaders remain persistent on multiple re-launches even after shaders for the game are mostly/fully built) "
"Otherwise on multiple playthroughs if you find a new shader you want to modify (or you missed one), you will need to delete game shader cache to trigger the shader compilation again. "
"Shader replacements should remain persistent even after shader cache deletions and multiple launches, the only time they become invalid is if the hash bytecode changes. "
"\n"
"\n PLEASE READ DOCUMENTATION FOR MORE DETAILS OR HELP!!!";

	static const char* tooltipRefreshShaderReplacements = 
"Refreshes the view in the 'ShaderInjector/ShaderReplacements' folder for shader replacements that you created. ";

	static const char* tooltipEnableShaderReplacement =
"Enable/disables the currently selected shader replacement. ";

	static const char* tooltipDeleteShaderReplacement =
"Deletes the currently selected shader replacement (NO UNDO!)";

	static const char* tooltipShaderSourcesCombobox =
"List of modified shaders (uncompiled) to choose from to apply to your current replacement shader. Located in 'ShaderInjector/ShaderSources'";

	static const char* tooltipRefreshShaderSourcesButton =
"Refreshes the view into 'ShaderInjector/ShaderSources' for any new modified shader files that you added/removed.";

	static const char* tooltipShaderReplacementRebuildButton =
"This applies the shader replacement (compiles selected source shader, rebuilds PSO, saves results)";

	static const char* tooltipSelectonStyle = 
"When a shader is selected it will be 'marked' in a couple of different ways, either with a blue pixel shader, or by hiding it.";

	static const char* tooltipClearSelections = 
"Clears any leftover selections leaving objects marked. ";

	static const char* tooltipStreamPipelinesHeader =
"Section that reveals all PSOs caught by the injector through 'D3D12_PIPELINE_STATE_STREAM'";

	static const char* tooltipCreateReplacementShaderTemplate =
"Creates a replacement shader template for the current selected shader, allowing it to be modified with new bytecode in 'Shader Replacements' menu up above.";

	static const char* tooltipDumpBytecode =
"Dumps raw shader bytecode to the disk in 'ShaderInjector/Dumps'";
}
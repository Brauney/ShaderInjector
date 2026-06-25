#include "ShaderInjectorGUIShaderSources.h"

#include <algorithm>
#include <string>
#include <vector>

#include "imgui.h"

#include "HookD3D12.h"
#include "ShaderInjectorIO.h"

namespace ShaderInjectorGUI
{
	static const float indentSpace = 24.0f;
	static ShaderReplacement::ShaderType gShaderSourceListType = ShaderReplacement::Unknown;
	static std::vector<std::string> gShaderSourceFiles;

	static std::string ShaderSourceSubdirectoryForType(ShaderReplacement::ShaderType shaderType)
	{
		return ShaderReplacement::ShaderTypeToString(shaderType) + "s";
	}

	static std::string ResolveShaderSourcePath(ShaderReplacement::ShaderType shaderType, const std::string& shaderSourceName)
	{
		if (shaderSourceName.empty())
			return "";

		return ShaderInjectorIO::GetShaderSourcesDirectory(ShaderSourceSubdirectoryForType(shaderType)) + "\\" + shaderSourceName;
	}

	static void SyncReplacementShaderSourcePath(ShaderReplacement::ShaderReplacementDisk& replacement)
	{
		if (replacement.shaderSourceName.empty() && !replacement.shaderSourcePath.empty())
			replacement.shaderSourceName = ShaderInjectorIO::FileNameFromPath(replacement.shaderSourcePath);

		if (!replacement.shaderSourceName.empty())
			replacement.shaderSourcePath = ResolveShaderSourcePath(replacement.shaderType, replacement.shaderSourceName);
	}

	static void RefreshShaderSources(ShaderReplacement::ShaderType shaderType)
	{
		gShaderSourceFiles.clear();
		gShaderSourceListType = shaderType;

		const std::string directory = ShaderInjectorIO::GetShaderSourcesDirectory(ShaderSourceSubdirectoryForType(shaderType));
		ShaderInjectorIO::DirectoryCreate(ShaderInjectorIO::GetShaderSourcesDirectory());
		ShaderInjectorIO::DirectoryCreate(directory);

		ShaderInjectorIO::CollectFilesByExtension(directory, ShaderInjectorIO::extensionHLSL, gShaderSourceFiles, false, false);
		std::sort(gShaderSourceFiles.begin(), gShaderSourceFiles.end());
	}

	void DrawShaderReplacementSourceSection(ShaderReplacement::ShaderReplacementDisk& replacement, int replacementIndex)
	{
		SyncReplacementShaderSourcePath(replacement);

		if (gShaderSourceListType != replacement.shaderType)
			RefreshShaderSources(replacement.shaderType);

		ImGui::SeparatorText("Replacement Shaders");
		ImGui::Indent(indentSpace);

		ImGui::Spacing();
		ImGui::Text("Source Folder: %s", ShaderInjectorIO::GetShaderSourcesDirectory(ShaderSourceSubdirectoryForType(replacement.shaderType)).c_str());
		ImGui::Text("Source Shader: ");
		ImGui::SameLine();

		const char* btnLabel = "Refresh Shader Sources";
		float buttonWidth = ImGui::CalcTextSize(btnLabel).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float comboWidth = ImGui::GetContentRegionAvail().x - buttonWidth - spacing;
		ImGui::SetNextItemWidth(comboWidth);

		const char* currentSource = replacement.shaderSourceName.empty() ? "(none)" : replacement.shaderSourceName.c_str();
		if (ImGui::BeginCombo("##ShaderReplacementSource", currentSource))
		{
			for (const std::string& shaderSourceFile : gShaderSourceFiles)
			{
				const bool selected = shaderSourceFile == replacement.shaderSourceName;

				if (ImGui::Selectable(shaderSourceFile.c_str(), selected))
				{
					replacement.shaderSourceName = shaderSourceFile;
					replacement.shaderSourcePath = ResolveShaderSourcePath(replacement.shaderType, replacement.shaderSourceName);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();
		if (ImGui::Button(btnLabel))
			RefreshShaderSources(replacement.shaderType);

		if (ImGui::Button("Rebuild Shader Replacement"))
		{
			HookD3D12::CompileShaderReplacement(replacementIndex);
			HookD3D12::ReloadShaderReplacement(replacementIndex);
			HookD3D12::SaveShaderReplacement(replacementIndex);
		}

		ImGui::Spacing();
		ImGui::Unindent(indentSpace);
	}
}

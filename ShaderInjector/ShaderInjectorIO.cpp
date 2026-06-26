	//ShaderInjectorIO.cpp
#include "ShaderInjectorIO.h"

#include <Windows.h>
#include <io.h>
#include <vector>
#include <fstream>
#include <cstring>

//3RD Party
#include "inicpp.h"

//custom
#include "ShaderTemplates.h"
#include "Globals.h"

namespace ShaderInjectorIO
{
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| IO HELPERS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| IO HELPERS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| IO HELPERS |||||||||||||||||||||||||||||||||||||||||||||||||||||

	bool PathExists(const std::string& filePath)
	{
		DWORD attributes = GetFileAttributesA(filePath.c_str());
		return attributes != INVALID_FILE_ATTRIBUTES;
	}

	bool FileExists(const std::string& filePath)
	{
		DWORD attributes = GetFileAttributesA(filePath.c_str());

		if (attributes == INVALID_FILE_ATTRIBUTES)
			return false;

		return !(attributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	void DeleteFileIfExists(const std::string& filePath)
	{
		if (!filePath.empty() && ShaderInjectorIO::FileExists(filePath))
			DeleteFileA(filePath.c_str());
	}

	bool WriteBinaryFile(const std::string& filePath, const void* data, size_t size)
	{
		if (!data || size == 0)
			return false;

		FILE* fileHandle = nullptr;
		fopen_s(&fileHandle, filePath.c_str(), "wb");

		if (!fileHandle)
			return false;

		const size_t bytesWritten = fwrite(data, 1, size, fileHandle);
		fclose(fileHandle);

		return bytesWritten == size;
	}

	bool WriteTextFileIfMissing(const std::string& filePath, const std::string& text)
	{
		if (FileExists(filePath))
			return true;

		std::ofstream file(filePath, std::ios::out | std::ios::trunc);

		if (!file.is_open())
			return false;

		file << text;
		return !file.fail();
	}

	bool DirectoryExists(const std::string& directoryPath)
	{
		DWORD attributes = GetFileAttributesA(directoryPath.c_str());

		if (attributes == INVALID_FILE_ATTRIBUTES)
			return false;

		return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	void DirectoryCreate(const std::string& directoryPath)
	{
		CreateDirectoryA(directoryPath.c_str(), nullptr);
	}

	std::string DirectoryFromPath(const std::string& path)
	{
		const size_t slash = path.find_last_of("\\/");
		return slash == std::string::npos ? "" : path.substr(0, slash);
	}

	std::string FileNameFromPath(const std::string& path)
	{
		const size_t slash = path.find_last_of("\\/");
		return slash == std::string::npos ? path : path.substr(slash + 1);
	}

	bool IsAbsolutePath(const std::string& path)
	{
		if (path.size() >= 3 && path[1] == ':' && (path[2] == '\\' || path[2] == '/'))
			return true;

		return path.size() >= 2 && ((path[0] == '\\' && path[1] == '\\') || (path[0] == '/' && path[1] == '/'));
	}

	void CollectFilesByExtension(const std::string& directory, const std::string& extension,std::vector<std::string>& outFiles, bool recursive, bool includeFullPath)
	{
		if (directory.empty() || extension.empty())
			return;

		WIN32_FIND_DATAA findData{};
		const std::string filePattern = directory + "\\*" + extension;
		HANDLE findHandle = FindFirstFileA(filePattern.c_str(), &findData);

		if (findHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					outFiles.push_back(includeFullPath ? directory + "\\" + findData.cFileName : findData.cFileName);
			} while (FindNextFileA(findHandle, &findData));

			FindClose(findHandle);
		}

		if (!recursive)
			return;

		const std::string childPattern = directory + "\\*";
		findHandle = FindFirstFileA(childPattern.c_str(), &findData);

		if (findHandle == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;

			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
				continue;

			CollectFilesByExtension(directory + "\\" + findData.cFileName, extension, outFiles, true, includeFullPath);
		} while (FindNextFileA(findHandle, &findData));

		FindClose(findHandle);
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| DIRECTORIES |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| DIRECTORIES |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| DIRECTORIES |||||||||||||||||||||||||||||||||||||||||||||||||||||

	std::string GetGameDirectory()
	{
		char path[MAX_PATH] = {};

		GetModuleFileNameA(nullptr, path, MAX_PATH);

		char* lastSlash = strrchr(path, '\\');

		if (lastSlash)
			*lastSlash = '\0';

		return path;
	}

	std::string GetShaderInjectorDirectory()
	{
		return GetGameDirectory() + "\\ShaderInjector";
	}

	std::string GetLogsDirectory()
	{
		return GetShaderInjectorDirectory() + "\\Logs";
	}

	std::string GetLogFilePath()
	{
		return GetLogsDirectory() + "\\ShaderInjector" + extensionLOG;
	}

	std::string GetDumpsDirectory()
	{
		return GetShaderInjectorDirectory() + "\\Dumps";
	}

	std::string GetUncapturedPSODirectory()
	{
		return GetShaderInjectorDirectory() + "\\UncapturedPSOs";
	}

	std::string GetToolsDirectory()
	{
		return GetShaderInjectorDirectory() + "\\Tools";
	}

	std::string GetToolPathDXC()
	{
		return GetToolsDirectory() + "\\dxc" + extensionEXE;
	}

	std::string GetShaderReplacementsDirectory()
	{
		return GetShaderInjectorDirectory() + "\\ShaderReplacements";
	}

	std::string GetShaderSourcesDirectory()
	{
		return GetShaderInjectorDirectory() + "\\ShaderSources";
	}

	std::string GetShaderSourcesDirectory(const std::string& shaderTypeDirectoryName)
	{
		return GetShaderSourcesDirectory() + "\\" + shaderTypeDirectoryName;
	}

	std::string GetInjectorSettingsPath()
	{
		return GetGameDirectory() + "\\" + injectorSettingsName;
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| LOGS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| LOGS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| LOGS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//TODO: a current and a previous log file

	//clears log file contents
	void PurgeLogFile()
	{
		FILE* logFileHandle = nullptr;

		fopen_s(&logFileHandle, GetLogFilePath().c_str(), "w");

		if (logFileHandle)
			fclose(logFileHandle);
	}


	void WriteToLogFile(const std::string& text)
	{
		FILE* logFileHandle = nullptr;

		//append to file, or write additively (if there isn't one, it will be created automatically)
		fopen_s(&logFileHandle, GetLogFilePath().c_str(), "a");

		if (!logFileHandle)
			return;

		SYSTEMTIME systemTime;
		GetLocalTime(&systemTime);

		fprintf(
			logFileHandle,
			"[%02d:%02d:%02d] %s\n", //write timestamp before each line
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			text.c_str());

		fclose(logFileHandle);
	}

	//quick wrapper, just to knock down our log strings in the code because my eyes hurt
	void WriteToLogFileError(const std::string& text)
	{
		WriteToLogFile("[ERROR] " + text);
	}

	//quick wrapper, just to knock down our log strings in the code because my eyes hurt
	void WriteToLogFileSuccess(const std::string& text)
	{
		WriteToLogFile("[SUCCESS] " + text);
	}

	//quick wrapper, just to knock down our log strings in the code because my eyes hurt
	void WriteToLogFileWarning(const std::string& text)
	{
		WriteToLogFile("[WARNING] " + text);
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| TOOLS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| TOOLS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| TOOLS |||||||||||||||||||||||||||||||||||||||||||||||||||||

	//NOTE: make this linux freindly!
	bool RunProcess(const std::string& commandLine)
	{
		STARTUPINFOA startupInformation{};
		PROCESS_INFORMATION processInformation{};

		startupInformation.cb = sizeof(startupInformation);

		char commandBuffer[4096];
		strcpy_s(commandBuffer, commandLine.c_str());

		BOOL processCreated = CreateProcessA(
			nullptr,
			commandBuffer,
			nullptr,
			nullptr,
			FALSE,
			CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&startupInformation,
			&processInformation);

		if (!processCreated)
			return false;

		WaitForSingleObject(processInformation.hProcess, INFINITE);

		DWORD exitCode = 0;
		GetExitCodeProcess(processInformation.hProcess, &exitCode);

		CloseHandle(processInformation.hThread);
		CloseHandle(processInformation.hProcess);

		return exitCode == 0;
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER |||||||||||||||||||||||||||||||||||||||||||||||||||||

	//given the file path of a compiled shader bytecode blob, use dxc to disassemble into a somewhat readable dxil text file
	//NOTE: make this linux freindly!
	bool GenerateShaderTextDXIL(const std::string shaderBytecodeFilePath)
	{
		if (!FileExists(shaderBytecodeFilePath))
		{
			WriteToLogFileError("ShaderInjectorIO->GenerateShaderTextDXIL: error! shader bytecode file not found! " + shaderBytecodeFilePath);
			return false;
		}

		std::string dxcPath = GetToolPathDXC();

		if (!FileExists(dxcPath))
		{
			WriteToLogFileError("ShaderInjectorIO->GenerateShaderTextDXIL: error! dxc.exe was not found!");
			return false;
		}

		std::string txtPath = shaderBytecodeFilePath.substr(0, shaderBytecodeFilePath.size() - 4) + extensionDXIL;
		std::string command =
			"cmd.exe /C \"\"" +
			dxcPath +
			"\" -dumpbin \"" +
			shaderBytecodeFilePath +
			"\" > \"" +
			txtPath +
			"\"\"";

		char commandBuffer[4096];
		strcpy_s(commandBuffer, command.c_str());

		//quick sanity check to see what the final command looks like
		//MessageBoxA(nullptr, commandBuffer, "DXC Command", MB_OK);

		RunProcess(commandBuffer);

		return true;
	}

	//given raw bytecode from memory, serialize/dump it to the disk in a given directory
	bool DumpShaderBytecode(const void* bytecode, size_t size, uint64_t hash, const std::string namePrefix, const std::string& directory)
	{
		if (!bytecode || size == 0)
		{
			WriteToLogFileError("ShaderInjectorIO->DumpShaderBytecode: error! given shader bytecode is null or size is 0!");
			return false;
		}

		char filename[256];
		sprintf_s(filename, "%016llX.bin", (unsigned long long)hash);
		std::string path = directory + "\\" + namePrefix + "_" + filename;

		FILE* shaderBytecodeFileHandle = nullptr;

		fopen_s(&shaderBytecodeFileHandle, path.c_str(), "wb");

		if (!shaderBytecodeFileHandle)
			return false;

		fwrite(bytecode, 1, size, shaderBytecodeFileHandle);
		fclose(shaderBytecodeFileHandle);

		return GenerateShaderTextDXIL(path);
	}

	//given raw HLSL human readable shader source code, compile it into a shader blob
	bool CompileSourceToDXILBlob(const std::string& shaderSourceFilePath, const std::string& shaderProfile, const std::string& entryPoint, std::string& outBlobPath)
	{
		if (!FileExists(shaderSourceFilePath))
		{
			WriteToLogFileError("ShaderInjectorIO->CompileSourceToDXILBlob: error! shader source file not found! " + shaderSourceFilePath);
			return false;
		}

		std::string dxcPath = GetToolPathDXC();

		if (!FileExists(dxcPath))
		{
			WriteToLogFileError("ShaderInjectorIO->CompileSourceToDXILBlob: error! dxc.exe was not found!");
			return false;
		}

		if (outBlobPath.empty())
			outBlobPath = shaderSourceFilePath.substr(0, shaderSourceFilePath.find_last_of('.')) + ".blob";

		char commandBuffer[4096];

		//call dxc through cmd.exe so quoted game-directory paths are handled consistently.
		sprintf_s(
			commandBuffer,
			"cmd.exe /C \"\"%s\" -T %s -E %s \"%s\" -Fo \"%s\"\"",
			dxcPath.c_str(),
			shaderProfile.c_str(),
			entryPoint.c_str(),
			shaderSourceFilePath.c_str(),
			outBlobPath.c_str());

		//quick sanity check to see what the final command looks like
		//MessageBoxA(nullptr, commandBuffer, "DXC Compile Command", MB_OK);

		return RunProcess(commandBuffer);
	}

	//given the path of a compiled shader blob, load it into memory
	bool LoadDXILBlobFromDisk(const std::string& shaderBlobFilePath, std::vector<uint8_t>& outBlob)
	{
		if (!FileExists(shaderBlobFilePath))
		{
			WriteToLogFileError("ShaderInjectorIO->LoadDXILBlobFromDisk: error! shader blob file not found! " + shaderBlobFilePath);
			return false;
		}

		// Clear stale bytes before loading the compiled DXIL blob into memory.
		outBlob.clear();

		FILE* shaderBlobFileHandle = nullptr;

		fopen_s(&shaderBlobFileHandle, shaderBlobFilePath.c_str(), "rb");

		if (!shaderBlobFileHandle)
			return false;

		fseek(shaderBlobFileHandle, 0, SEEK_END);

		long fileSize = ftell(shaderBlobFileHandle);

		fseek(shaderBlobFileHandle, 0, SEEK_SET);

		if (fileSize <= 0)
		{
			fclose(shaderBlobFileHandle);
			return false;
		}

		outBlob.resize((size_t)fileSize);

		fread(outBlob.data(), 1, (size_t)fileSize, shaderBlobFileHandle);

		fclose(shaderBlobFileHandle);

		return true;
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER INTERNAL RESOURCES |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER INTERNAL RESOURCES |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| SHADER INTERNAL RESOURCES |||||||||||||||||||||||||||||||||||||||||||||||||||||

	std::string GetInternalMarkerPixelShaderSourceCodeFilePath() 
	{
		return GetShaderInjectorDirectory() + "\\" + internalMarkerPixelShaderName + extensionHLSL;
	}

	std::string GetInternalMarkerPixelShaderBlobFilePath()
	{
		return GetShaderInjectorDirectory() + "\\" + internalMarkerPixelShaderName + extensionBLOB;
	}

	std::string GetInternalNullPixelShaderSourceCodeFilePath()
	{
		return GetShaderInjectorDirectory() + "\\" + internalNullPixelShaderName + extensionHLSL;
	}

	std::string GetInternalNullPixelShaderBlobFilePath()
	{
		return GetShaderInjectorDirectory() + "\\" + internalNullPixelShaderName + extensionBLOB;
	}

	std::string GetInternalMarkerComputeShaderSourceCodeFilePath()
	{
		return GetShaderInjectorDirectory() + "\\" + internalMarkerComputeShaderName + extensionHLSL;
	}

	std::string GetInternalMarkerComputeShaderBlobFilePath()
	{
		return GetShaderInjectorDirectory() + "\\" + internalMarkerComputeShaderName + extensionBLOB;
	}

	bool WriteInternalShaderSourceCodeToDisk(std::string shaderSourceFilePath, const char* shaderSourceCode)
	{
		//NOTE: we open with trunc so we overwrite what was originally there in the file
		//for "internal" shaders used by the shader injector, want to make sure that we are starting clean and fresh
		//as it's very possible that it got tampered with
		std::ofstream file(shaderSourceFilePath, std::ios::out | std::ios::trunc);

		if (!file.is_open())
		{
			WriteToLogFileError("ShaderInjectorIO->WriteInternalShaderCodeToDisk: failed to open file: " + shaderSourceFilePath);
			return false;
		}

		file << shaderSourceCode;

		if (file.fail())
		{
			WriteToLogFileError("ShaderInjectorIO->WriteInternalShaderCodeToDisk: failed to write file: " + shaderSourceFilePath);
			return false;
		}

		file.close();

		return true;
	}

	bool WriteInternalMarkerPixelShaderSourceCodeToDisk()
	{
		return WriteInternalShaderSourceCodeToDisk(GetInternalMarkerPixelShaderSourceCodeFilePath(), ShaderTemplates::internalMarkerPixelShaderSourceCode);
	}

	bool WriteInternalNullPixelShaderSourceCodeToDisk()
	{
		return WriteInternalShaderSourceCodeToDisk(GetInternalNullPixelShaderSourceCodeFilePath(), ShaderTemplates::internalNullPixelShaderSourceCode);
	}

	bool WriteInternalMarkerComputeShaderSourceCodeToDisk()
	{
		return WriteInternalShaderSourceCodeToDisk(GetInternalMarkerComputeShaderSourceCodeFilePath(), ShaderTemplates::internalMarkerComputeShaderSourceCode);
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| INJECTOR SETTINGS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| INJECTOR SETTINGS |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| INJECTOR SETTINGS |||||||||||||||||||||||||||||||||||||||||||||||||||||

	//returns true for success, false for failure
	bool ReadInjectorSettings() 
	{
		std::string injectorSettingsPath = GetInjectorSettingsPath();

		if (!FileExists(injectorSettingsPath))
		{
			WriteToLogFileWarning("ShaderInjectorIO->ReadInjectorSettings: injector settings ini not found! using default settings... ");
			return false;
		}

		ini::IniFile injectorSettingsINI;
		injectorSettingsINI.load(injectorSettingsPath);

		int keyOpenShaderInjectorGUI = injectorSettingsINI["InjectorSettings"]["OpenMenuKey"].as<int>();
		int keyToggleShaderInjector = injectorSettingsINI["InjectorSettings"]["ToggleInjectorKey"].as<int>();
		bool gShaderInjectorEnabled = injectorSettingsINI["InjectorSettings"]["InjectorEnabled"].as<bool>();
		bool gShowShaderInjectorGUI = injectorSettingsINI["InjectorSettings"]["MenuOpen"].as<bool>();

		Globals::keyOpenShaderInjectorGUI = keyOpenShaderInjectorGUI;
		Globals::keyToggleShaderInjector = keyToggleShaderInjector;
		Globals::gShaderInjectorEnabled = gShaderInjectorEnabled;
		Globals::gShowShaderInjectorGUI = gShowShaderInjectorGUI;

		WriteToLogFile("ShaderInjectorIO->ReadInjectorSettings: keyOpenShaderInjectorGUI " + std::to_string(keyOpenShaderInjectorGUI));
		WriteToLogFile("ShaderInjectorIO->ReadInjectorSettings: keyToggleShaderInjector " + std::to_string(keyToggleShaderInjector));
		WriteToLogFile("ShaderInjectorIO->ReadInjectorSettings: gShaderInjectorEnabled " + std::to_string(gShaderInjectorEnabled));
		WriteToLogFile("ShaderInjectorIO->ReadInjectorSettings: gShowShaderInjectorGUI " + std::to_string(gShowShaderInjectorGUI));

		WriteToLogFile("ShaderInjectorIO->ReadInjectorSettings: parsed injector settings");

		return true;
	}

	void CreateInjectorSettings() 
	{
		std::string injectorSettingsPath = GetInjectorSettingsPath();

		if (FileExists(injectorSettingsPath))
		{
			WriteToLogFileWarning("ShaderInjectorIO->CreateInjectorSettings: injector settings int already exists!");
			return;
		}

		ini::IniFile injectorSettingsINI;
		injectorSettingsINI["InjectorSettings"]["OpenMenuKey"] = Globals::keyOpenShaderInjectorGUI;
		injectorSettingsINI["InjectorSettings"]["ToggleInjectorKey"] = Globals::keyToggleShaderInjector;
		injectorSettingsINI["InjectorSettings"]["InjectorEnabled"] = Globals::gShaderInjectorEnabled;
		injectorSettingsINI["InjectorSettings"]["MenuOpen"] = Globals::gShowShaderInjectorGUI;

		std::ofstream file(injectorSettingsPath, std::ios::out | std::ios::trunc);

		if (!file.is_open())
		{
			WriteToLogFileError("ShaderInjectorIO->CreateInjectorSettings: failed to open injector settings: " + injectorSettingsPath);
			return;
		}

		injectorSettingsINI.encode(file);

		file.close();

		WriteToLogFile("ShaderInjectorIO->CreateInjectorSettings: new injector settings created. ");
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||| INITALIZE |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| INITALIZE |||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||| INITALIZE |||||||||||||||||||||||||||||||||||||||||||||||||||||

	bool Initialize()
	{
		WriteToLogFile("ShaderInjectorIO->Initialize: Initalizing...");

		std::string shaderInjectorDirectory = GetShaderInjectorDirectory();
		std::string logsDirectory = GetLogsDirectory();
		std::string toolsDirectory = GetToolsDirectory();
		std::string dumpDirectory = GetDumpsDirectory();
		std::string uncapturedPSODirectory = GetUncapturedPSODirectory();
		std::string shaderReplacementsDirectory = GetShaderReplacementsDirectory();
		std::string shaderSourcesDirectory = GetShaderSourcesDirectory();
		std::string injectorSettingsPath = GetInjectorSettingsPath();

		//========================= INJECTOR SETTINGS =========================
		//start by reading (or creating) injector settings to configure the injector

		bool injectorSettingsReadResult = ReadInjectorSettings();

		//check if there is existing injetor settings
		if (!injectorSettingsReadResult)
		{
			if(!FileExists(injectorSettingsPath))
				CreateInjectorSettings();
		}

		//========================= CREATE DIRECTORY STRUCTURE =========================
		//if the directory structure or folders do not exist... that will become a problem later...
		//but just as a helpful measure we will go ahead and build it out

		if (!DirectoryExists(shaderInjectorDirectory))
		{
			DirectoryCreate(shaderInjectorDirectory);
			WriteToLogFileWarning("ShaderInjectorIO->Initalize: " + shaderInjectorDirectory + " did not exist! Created anyway.");
		}

		if (!DirectoryExists(logsDirectory))
		{
			DirectoryCreate(logsDirectory);
			WriteToLogFile("ShaderInjectorIO->Initalize: " + logsDirectory + " did not exist! Created anyway.");
		}

		if (!DirectoryExists(toolsDirectory))
		{
			DirectoryCreate(toolsDirectory);
			WriteToLogFileWarning("ShaderInjectorIO->Initalize: " + toolsDirectory + " did not exist! Created anyway.");
		}

		if (!DirectoryExists(dumpDirectory))
		{
			DirectoryCreate(dumpDirectory);
			WriteToLogFile("ShaderInjectorIO->Initalize: " + dumpDirectory + " did not exist! Created anyway.");
		}

		if (!DirectoryExists(uncapturedPSODirectory))
		{
			DirectoryCreate(uncapturedPSODirectory);
			WriteToLogFile("ShaderInjectorIO->Initalize: " + uncapturedPSODirectory + " did not exist! Created anyway.");
		}

		if (!DirectoryExists(shaderReplacementsDirectory))
		{
			DirectoryCreate(shaderReplacementsDirectory);
			WriteToLogFile("ShaderInjectorIO->Initalize: " + shaderReplacementsDirectory + " did not exist! Created anyway.");
		}

		if (!DirectoryExists(shaderSourcesDirectory))
		{
			DirectoryCreate(shaderSourcesDirectory);
			WriteToLogFileWarning("ShaderInjectorIO->Initalize: " + shaderSourcesDirectory + " did not exist! Created anyway.");
		}

		//create sudirectories within shader injector sources for the different shader types that we can potentially create
		const char* shaderSourceSubdirectories[] =
		{
			"VertexShaders",
			"HullShaders",
			"DomainShaders",
			"GeometryShaders",
			"PixelShaders",
			"ComputeShaders",
		};

		for (const char* shaderSourceSubdirectory : shaderSourceSubdirectories)
		{
			std::string shaderSourceDirectory = GetShaderSourcesDirectory(shaderSourceSubdirectory);

			if (!DirectoryExists(shaderSourceDirectory))
			{
				DirectoryCreate(shaderSourceDirectory);
				WriteToLogFileWarning("ShaderInjectorIO->Initalize: " + shaderSourceDirectory + " did not exist! Created anyway.");
			}
		}

		//========================= CREATE INTERNAL SHADER FILES =========================
		//now go ahead and create the internal shader files that the injector uses to mark shaders
		//NOTE TO SELF 1: we should make this a configuration as doing this every time we boot up will slow startup times
		//NOTE TO SELF 2: also do file checks where if the file does not exist, that is when we force remake them
		//NOTE TO SELF 3: if earlier we built a fresh directory/folder structure, then we won't have dxc.exe so compiling these will fail

		WriteInternalMarkerPixelShaderSourceCodeToDisk();
		WriteInternalNullPixelShaderSourceCodeToDisk();
		WriteInternalMarkerComputeShaderSourceCodeToDisk();

		std::string internalMarkerPixelShaderBlobPath = GetInternalMarkerPixelShaderBlobFilePath();
		std::string internalMarkerComputeShaderBlobPath = GetInternalMarkerComputeShaderBlobFilePath();

		bool internalMarkerPixelShaderBlobResult = CompileSourceToDXILBlob(
			GetInternalMarkerPixelShaderSourceCodeFilePath(),
			"ps_6_6", //target profile, for rebirth ps_6_6 is what I found in renderdoc
			"main", //name of the function within the shader to execute
			internalMarkerPixelShaderBlobPath); //output blob file path

		bool internalMarkerComputeShaderBlobResult = CompileSourceToDXILBlob(
			GetInternalMarkerComputeShaderSourceCodeFilePath(),
			"cs_6_6", //target profile, for rebirth cs_6_6 is what I found in renderdoc
			"main", //name of the function within the shader to execute
			internalMarkerComputeShaderBlobPath); //output blob file path

		return true;
	}
}

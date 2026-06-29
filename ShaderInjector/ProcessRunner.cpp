// ProcessRunner.cpp
#include "ProcessRunner.h"

#include <cerrno>
#include <cstring>

#if defined(_WIN32)
	#include <Windows.h>
#else
	#include <fcntl.h>
	#include <sys/wait.h>
	#include <unistd.h>
#endif

namespace ProcessRunner
{
	namespace
	{
#if defined(_WIN32)
		std::wstring ToWideString(const std::string& text)
		{
			if (text.empty())
				return {};

			int characterCount = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1, nullptr, 0);
			UINT codePage = CP_UTF8;

			// Older replacement files may contain paths encoded with the active Windows
			// code page. Keep those files usable while all new paths are treated as UTF-8.
			if (characterCount == 0)
			{
				codePage = CP_ACP;
				characterCount = MultiByteToWideChar(codePage, 0, text.c_str(), -1, nullptr, 0);
			}

			if (characterCount == 0)
				return {};

			std::wstring result(static_cast<size_t>(characterCount), L'\0');
			MultiByteToWideChar(codePage, 0, text.c_str(), -1, result.data(), characterCount);
			result.pop_back();
			return result;
		}

		std::wstring QuoteWindowsArgument(const std::wstring& argument)
		{
			std::wstring quoted = L"\"";
			size_t backslashCount = 0;

			for (wchar_t character : argument)
			{
				if (character == L'\\')
				{
					++backslashCount;
					continue;
				}

				if (character == L'\"')
				{
					quoted.append(backslashCount * 2 + 1, L'\\');
					quoted.push_back(character);
					backslashCount = 0;
					continue;
				}

				quoted.append(backslashCount, L'\\');
				backslashCount = 0;
				quoted.push_back(character);
			}

			quoted.append(backslashCount * 2, L'\\');
			quoted.push_back(L'\"');
			return quoted;
		}

		std::string WindowsErrorMessage(DWORD errorCode)
		{
			char* messageBuffer = nullptr;
			const DWORD messageLength = FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				errorCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<char*>(&messageBuffer),
				0,
				nullptr);

			std::string message = messageLength && messageBuffer
				? std::string(messageBuffer, messageLength)
				: "Windows error " + std::to_string(errorCode);

			if (messageBuffer)
				LocalFree(messageBuffer);

			while (!message.empty() && (message.back() == '\r' || message.back() == '\n'))
				message.pop_back();

			return message;
		}
#endif
	}

	ProcessResult Run(
		const std::string& executablePath,
		const std::vector<std::string>& arguments,
		const std::string& standardOutputPath)
	{
		ProcessResult result{};

		if (executablePath.empty())
		{
			result.errorMessage = "Executable path is empty";
			return result;
		}

#if defined(_WIN32)
		const std::wstring wideExecutablePath = ToWideString(executablePath);
		if (wideExecutablePath.empty())
		{
			result.errorMessage = "Executable path could not be converted to UTF-16";
			return result;
		}

		std::wstring commandLine = QuoteWindowsArgument(wideExecutablePath);
		for (const std::string& argument : arguments)
		{
			commandLine.push_back(L' ');
			commandLine += QuoteWindowsArgument(ToWideString(argument));
		}

		std::vector<wchar_t> mutableCommandLine(commandLine.begin(), commandLine.end());
		mutableCommandLine.push_back(L'\0');

		STARTUPINFOW startupInformation{};
		PROCESS_INFORMATION processInformation{};
		startupInformation.cb = sizeof(startupInformation);

		HANDLE outputHandle = INVALID_HANDLE_VALUE;
		BOOL inheritHandles = FALSE;

		if (!standardOutputPath.empty())
		{
			SECURITY_ATTRIBUTES securityAttributes{};
			securityAttributes.nLength = sizeof(securityAttributes);
			securityAttributes.bInheritHandle = TRUE;

			outputHandle = CreateFileW(
				ToWideString(standardOutputPath).c_str(),
				GENERIC_WRITE,
				FILE_SHARE_READ,
				&securityAttributes,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr);

			if (outputHandle == INVALID_HANDLE_VALUE)
			{
				result.errorMessage = "Could not open process output file: " + WindowsErrorMessage(GetLastError());
				return result;
			}

			startupInformation.dwFlags |= STARTF_USESTDHANDLES;
			startupInformation.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			startupInformation.hStdOutput = outputHandle;
			startupInformation.hStdError = outputHandle;
			inheritHandles = TRUE;
		}

		const BOOL processCreated = CreateProcessW(
			wideExecutablePath.c_str(),
			mutableCommandLine.data(),
			nullptr,
			nullptr,
			inheritHandles,
			CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&startupInformation,
			&processInformation);

		if (outputHandle != INVALID_HANDLE_VALUE)
			CloseHandle(outputHandle);

		if (!processCreated)
		{
			result.errorMessage = WindowsErrorMessage(GetLastError());
			return result;
		}

		result.launched = true;
		WaitForSingleObject(processInformation.hProcess, INFINITE);

		DWORD exitCode = static_cast<DWORD>(-1);
		if (GetExitCodeProcess(processInformation.hProcess, &exitCode))
			result.exitCode = static_cast<int>(exitCode);
		else
			result.errorMessage = WindowsErrorMessage(GetLastError());

		CloseHandle(processInformation.hThread);
		CloseHandle(processInformation.hProcess);
#else
		const pid_t processId = fork();
		if (processId < 0)
		{
			result.errorMessage = std::strerror(errno);
			return result;
		}

		if (processId == 0)
		{
			if (!standardOutputPath.empty())
			{
				const int outputDescriptor = open(standardOutputPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (outputDescriptor < 0)
					_exit(126);

				dup2(outputDescriptor, STDOUT_FILENO);
				dup2(outputDescriptor, STDERR_FILENO);
				close(outputDescriptor);
			}

			std::vector<char*> argumentPointers;
			argumentPointers.reserve(arguments.size() + 2);
			argumentPointers.push_back(const_cast<char*>(executablePath.c_str()));
			for (const std::string& argument : arguments)
				argumentPointers.push_back(const_cast<char*>(argument.c_str()));
			argumentPointers.push_back(nullptr);

			execv(executablePath.c_str(), argumentPointers.data());
			_exit(errno == ENOENT ? 127 : 126);
		}

		result.launched = true;
		int processStatus = 0;
		while (waitpid(processId, &processStatus, 0) < 0)
		{
			if (errno == EINTR)
				continue;

			result.errorMessage = std::strerror(errno);
			return result;
		}

		if (WIFEXITED(processStatus))
			result.exitCode = WEXITSTATUS(processStatus);
		else if (WIFSIGNALED(processStatus))
			result.exitCode = 128 + WTERMSIG(processStatus);
#endif

		if (result.launched && result.exitCode != 0 && result.errorMessage.empty())
			result.errorMessage = "Process exited with code " + std::to_string(result.exitCode);

		return result;
	}
}

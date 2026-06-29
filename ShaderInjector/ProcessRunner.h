// ProcessRunner.h
#pragma once

#include <string>
#include <vector>

namespace ProcessRunner
{
	struct ProcessResult
	{
		bool launched = false;
		int exitCode = -1;
		std::string errorMessage;

		bool Succeeded() const
		{
			return launched && exitCode == 0;
		}
	};

	// Launches an executable directly without a command shell. Arguments are passed
	// individually so paths and user-controlled values never need shell escaping.
	ProcessResult Run(
		const std::string& executablePath,
		const std::vector<std::string>& arguments,
		const std::string& standardOutputPath = "");
}

#include "Hash.h"

#include <string>

namespace Hash
{
	uint64_t HashMemory(const void* data, size_t size)
	{
		const uint8_t* bytes = (const uint8_t*)data;
		uint64_t hash = 14695981039346656037ULL;

		for (size_t i = 0; i < size; ++i)
		{
			hash ^= bytes[i];
			hash *= 1099511628211ULL;
		}

		return hash;
	}

	std::string FormatHash(uint64_t hash)
	{
		char buffer[32]{};
		sprintf_s(buffer, "%016llX", (unsigned long long)hash);
		return buffer;
	}

	uint64_t ParseHashText(const std::string& hashText)
	{
		if (hashText.empty())
			return 0;

		char* end = nullptr;
		return strtoull(hashText.c_str(), &end, 16);
	}
}
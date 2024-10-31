#pragma once

#include "Sleet/Core/Base.h"

namespace Sleet {

	namespace Utils {

		static uint32_t AlignedSize(uint32_t value, uint32_t alignment)
		{
			return (value + alignment - 1) & ~(alignment - 1);
		}

		static std::string GetParentDirectory(const std::string& filepath) 
		{
			size_t pos = filepath.find_last_of("/\\");
			if (pos == std::string::npos) 
			{
				return ""; // Return empty string if no directory separator is found
			}
			return filepath.substr(0, pos);
		}

	}

}
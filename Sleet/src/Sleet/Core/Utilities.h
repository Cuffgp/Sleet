#pragma once

#include "Sleet/Core/Base.h"

namespace Sleet {

	namespace Utils {

		static uint32_t AlignedSize(uint32_t value, uint32_t alignment)
		{
			return (value + alignment - 1) & ~(alignment - 1);
		}

	}

}
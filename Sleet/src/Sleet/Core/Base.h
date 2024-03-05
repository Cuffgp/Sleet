#pragma once

#include <memory>
#include <string>

#define SL_ENABLE_ASSERTS

#ifdef SL_ENABLE_ASSERTS
#define SL_ASSERT(cond, msg) {if(!cond) {SL_ERROR("{}", msg); __debugbreak();}}
#else
#define SL_ASSERT(...)
#endif

#define SL_BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define BIT(x) (1 << x)

#ifndef NOMINMAX
#define NOMINMAX
#endif

namespace Sleet {

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	using byte = uint8_t;

}

#include "Sleet/Core/Log.h"
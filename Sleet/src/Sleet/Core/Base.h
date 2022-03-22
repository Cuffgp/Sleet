#pragma once

#include <memory>
#include <filesystem>

#define SL_BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define SL_DEBUGBREAK() __debugbreak()
#define SL_EXPAND_MACRO(x) x
#define SL_STRINGIFY_MACRO(x) #x

#define SL_ENABLE_ASSERTS
#ifdef SL_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define SL_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { SL##type##ERROR(msg, __VA_ARGS__); SL_DEBUGBREAK(); } }
#define SL_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SL_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define SL_INTERNAL_ASSERT_NO_MSG(type, check) SL_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SL_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define SL_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define SL_INTERNAL_ASSERT_GET_MACRO(...) SL_EXPAND_MACRO( SL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, SL_INTERNAL_ASSERT_WITH_MSG, SL_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define SL_ASSERT(...) SL_EXPAND_MACRO( SL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )

#else
#define SL_ASSERT(...)
#endif

namespace Sleet {
	// Unique and Shared pointsers to Scope<> and Ref<>
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	using byte = uint8_t;

}
#pragma once

#include "Sleet/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include "spdlog/spdlog.h"

namespace Sleet {

	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
	};

}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core log macros
#define SL_TRACE(...)    ::Sleet::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SL_INFO(...)     ::Sleet::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SL_WARN(...)     ::Sleet::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SL_ERROR(...)    ::Sleet::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SL_CRITICAL(...) ::Sleet::Log::GetCoreLogger()->critical(__VA_ARGS__)


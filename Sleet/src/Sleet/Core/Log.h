#pragma once

#include "Sleet/Core/Base.h"

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

// Core log macros
#define SL_TRACE(...)    ::Sleet::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SL_INFO(...)     ::Sleet::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SL_WARN(...)     ::Sleet::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SL_ERROR(...)    ::Sleet::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SL_CRITICAL(...) ::Sleet::Log::GetCoreLogger()->critical(__VA_ARGS__)


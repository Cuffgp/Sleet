#pragma once

#include <memory>
#include "Sleet/Core/Base.h"

#include "spdlog/spdlog.h"

namespace Sleet {

	class Log
	{
	public:
		static void Init();
		static Ref<spdlog::logger> GetLogger() { return s_Logger; }
	private:
		static Ref<spdlog::logger> s_Logger;
	};

}

#define SL_TRACE(...) ::Sleet::Log::GetLogger()->trace(__VA_ARGS__)
#define SL_INFO(...) ::Sleet::Log::GetLogger()->info(__VA_ARGS__)
#define SL_WARN(...) ::Sleet::Log::GetLogger()->warn(__VA_ARGS__)
#define SL_ERROR(...) ::Sleet::Log::GetLogger()->error(__VA_ARGS__)
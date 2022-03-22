#include "slpch.h"
#include "Sleet/Core/Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Sleet {

	Ref<spdlog::logger> Log::s_Logger = nullptr;

	void Log::Init()
	{
		auto console_sink = CreateRef<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_pattern("%^ [%T] [%n]: %v %$");

		s_Logger = CreateRef<spdlog::logger>("Sleet", console_sink);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
	}

}
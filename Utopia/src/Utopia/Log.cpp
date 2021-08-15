#include "utpch.hpp"
#include "Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>


namespace Utopia
{
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T](%l) %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("UTOPIA");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}

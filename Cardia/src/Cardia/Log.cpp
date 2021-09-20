#include "cdpch.hpp"
#include "Cardia/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>


namespace Cardia
{
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T](%l) %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("CARDIA");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}

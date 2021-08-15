#pragma once

#include "Core.hpp"

// TODO: Remove deps to spdlog
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Utopia
{
	class UTOPIA_API Logger
	{
	public:
		static void Init();
		static spdlog::logger* GetCoreLogger() { return s_CoreLogger.get(); }
		static spdlog::logger* GetClientLogger() { return s_ClientLogger.get(); }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

	namespace Log
	{
		// Core Log
		template<typename... Args>
		constexpr void coreTrace(Args... args)
		{
			Logger::GetCoreLogger()->trace(args...);
		}

		template<typename... Args>
		constexpr void coreInfo(Args... args)
		{
			Logger::GetCoreLogger()->info(args...);
		}

		template<typename... Args>
		constexpr void coreWarn(Args... args)
		{
			Logger::GetCoreLogger()->warn(args...);
		}

		template<typename... Args>
		constexpr void coreError(Args... args)
		{
			Logger::GetCoreLogger()->error(args...);
		}

		// SandBox Log Macros
		template<typename... Args>
		constexpr void trace(Args... args)
		{
			Logger::GetClientLogger()->trace(args...);
		}

		template<typename... Args>
		constexpr void info(Args... args)
		{
			Logger::GetClientLogger()->info(args...);
		}

		template<typename... Args>
		constexpr void warn(Args... args)
		{
			Logger::GetClientLogger()->warn(args...);
		}

		template<typename... Args>
		constexpr void error(Args... args)
		{
			Logger::GetClientLogger()->error(args...);
		}
	}
}

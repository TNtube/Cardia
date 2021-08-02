#pragma once

#include <memory>
#include "Core.h"
#include "spdlog/spdlog.h"

namespace Utopia
{
	class UTOPIA_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core Log Macros
#define UT_CORE_TRACE(...)		::Utopia::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define UT_CORE_INFO(...)		::Utopia::Log::GetCoreLogger()->info(__VA_ARGS__)
#define UT_CORE_WARN(...)		::Utopia::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define UT_CORE_ERROR(...)		::Utopia::Log::GetCoreLogger()->error(__VA_ARGS__)
#define UT_CORE_FATAL(...)		::Utopia::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Core Log Macros
#define UT_TRACE(...)			::Utopia::Log::GetClientLogger()->trace(__VA_ARGS__)
#define UT_INFO(...)			::Utopia::Log::GetClientLogger()->info(__VA_ARGS__)
#define UT_WARN(...)			::Utopia::Log::GetClientLogger()->warn(__VA_ARGS__)
#define UT_ERROR(...)			::Utopia::Log::GetClientLogger()->error(__VA_ARGS__)
#define UT_FATAL(...)			::Utopia::Log::GetClientLogger()->fatal(__VA_ARGS__)
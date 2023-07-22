#pragma once

#include <spdlog/spdlog.h>


namespace Cardia
{
	typedef std::function<void(const spdlog::level::level_enum& level, const std::string& msg)> custom_log_callback;
	class Logger
	{
	public:
		static void Init();
		static spdlog::logger* GetCoreLogger() { return s_CoreLogger.get(); }
		static spdlog::logger* GetEditorLogger() { return s_ClientLogger.get(); }
		static void AddCoreCallback(const custom_log_callback& callback);
		static void AddEditorCallback(const custom_log_callback& callback);
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

	namespace Log
	{
		// Core Log
		template<typename... Args>
		constexpr void CoreTrace(Args&&... args)
		{
			Logger::GetEditorLogger();
			Logger::GetCoreLogger()->trace(std::forward<Args>(args)...);
		}

		template<typename... Args>
		constexpr void CoreInfo(Args&&... args)
		{
			Logger::GetCoreLogger()->info(std::forward<Args>(args)...);
		}

		template<typename... Args>
		constexpr void CoreWarn(Args&&... args)
		{
			Logger::GetCoreLogger()->warn(std::forward<Args>(args)...);
		}

		template<typename... Args>
		constexpr void CoreError(Args&&... args)
		{
			Logger::GetCoreLogger()->error(std::forward<Args>(args)...);
		}

		// SandBox Log Macros
		template<typename... Args>
		constexpr void Trace(Args&&... args)
		{
			Logger::GetEditorLogger()->trace(std::forward<Args>(args)...);
		}

		template<typename... Args>
		constexpr void Info(Args&&... args)
		{
			Logger::GetEditorLogger()->info(std::forward<Args>(args)...);
		}

		template<typename... Args>
		constexpr void Warn(Args&&... args)
		{
			Logger::GetEditorLogger()->warn(std::forward<Args>(args)...);
		}

		template<typename... Args>
		constexpr void Error(Args&&... args)
		{
			Logger::GetEditorLogger()->error(std::forward<Args>(args)...);
		}
	}
}

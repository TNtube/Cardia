#include "cdpch.hpp"
#include "Cardia/Core/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/base_sink.h>

#include <utility>


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

	template<typename Mutex>
	class callback_sink final : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		explicit callback_sink(custom_log_callback callback)
			: callback_{std::move(callback)}
		{}
	protected:
		void sink_it_(const spdlog::details::log_msg &msg) override
		{
			spdlog::memory_buf_t formatted;
			formatter.format(msg, formatted);
			auto eol_len = strlen(spdlog::details::os::default_eol);
			std::string str_msg(formatted.begin(), formatted.end() - eol_len);
			callback_(msg.level, str_msg + "\n");
		}
		void flush_() override {};
	private:
		custom_log_callback callback_;
		spdlog::pattern_formatter formatter;
	};

	void Logger::AddCoreCallback(const custom_log_callback &callback)
	{
		s_CoreLogger->sinks().push_back(std::make_shared<callback_sink<std::mutex>>(callback));
	}

	void Logger::AddEditorCallback(const custom_log_callback &callback)
	{
		s_CoreLogger->sinks().push_back(std::make_shared<callback_sink<std::mutex>>(callback));
	}
}

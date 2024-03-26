
#include "Runtime/Core/pch.h"

#include "debug.h"
#include <iostream>

#include "spdlog/fmt/bundled/compile.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/details/os.h"

namespace LitchiRuntime
{
	class callback_sink_mt : public spdlog::sinks::base_sink<spdlog::details::null_mutex> {
	public:
		callback_sink_mt() = delete;
		callback_sink_mt(std::function<void(const spdlog::details::log_msg&,const std::string&)> callback) : base_sink()
		{
			m_callback = callback;
		}

		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			spdlog::memory_buf_t formatted;
			formatter_->format(msg, formatted);
			std::string realMsg = formatted.data();
			int end = realMsg.find("\r\n");
			if(end!=-1)
			{
				realMsg = realMsg.substr(0, end);
			}
			m_callback(msg, realMsg);
		}

		void flush_() override
		{
			// do nothing
		}
	private:
		std::function<void(const spdlog::details::log_msg&, const std::string&)> m_callback;
	};

	Event<const LogData&> Debug::LogEvent;

	void Debug::Initialize() {
		try
		{
			// create console sink
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			console_sink->set_level(spdlog::level::trace);
			//        console_sink->set_pattern("[source %g] [function %!] [line %#] [%^%l%$] %v");

			// create file sink
			auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
			file_sink->set_level(spdlog::level::trace);

			// create callback sink

			auto callback_sink = std::make_shared<callback_sink_mt>([](const spdlog::details::log_msg& log_msg, const std::string& realMsg) {

				auto level = log_msg.level;
				auto logMsg = log_msg.payload;

				LogData logData;

				switch (level)
				{
					case spdlog::level::level_enum::debug:
					case spdlog::level::level_enum::info:
						logData.logLevel = ELogLevel::LOG_INFO;
						break;
					case spdlog::level::level_enum::warn:
						logData.logLevel = ELogLevel::LOG_WARNING;
						break;
					case spdlog::level::level_enum::err:
					case spdlog::level::level_enum::trace:
					case spdlog::level::level_enum::critical:
						logData.logLevel = ELogLevel::LOG_ERROR;
						break;
					default:
						logData.logLevel = ELogLevel::LOG_DEFAULT;
						break;
				}
				logData.message = realMsg;
				time_t tnow = spdlog::log_clock::to_time_t(log_msg.time);
				auto tm =  spdlog::details::os::localtime(tnow);
				
				logData.date = fmt::format("[{},{},{}]",tm.tm_hour,tm.tm_min,tm.tm_sec);
				LogEvent.Invoke(logData);
				});
			callback_sink->set_level(spdlog::level::trace);

			// you can even set multi_sink logger as default logger
			auto logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({ console_sink, file_sink, callback_sink }));
			spdlog::set_default_logger(logger);
			spdlog::set_pattern("[source %s] [function %!] [line %#] [%^%l%$] %v");
			spdlog::flush_on(spdlog::level::trace);// flush on every log call.


			DEBUG_LOG_INFO("spdlog init success {}", 1);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
		}
	}
}

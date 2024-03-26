
#include "debug.h"
#include <iostream>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
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
			const std::string realMsg = formatted.data();
			m_callback(msg, realMsg);
		}

		void flush_() override
		{
			// do nothing
		}

	private:
		std::function<void(const spdlog::details::log_msg&, const std::string&)> m_callback;
	};

	Event<ELogMode, const std::string&> Debug::LogEvent;

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

				ELogMode mode = ELogMode::DEFAULT;
				switch (level)
				{
				case spdlog::level::level_enum::debug:
					break;

				default:;
				}

				LogEvent.Invoke(mode, realMsg);
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

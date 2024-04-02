
#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "Runtime/Core/Tools/Eventing/Event.h"

#include "spdlog/spdlog.h"

/// 输出文件名
#define DEBUG_LOG_INFO SPDLOG_INFO
#define DEBUG_LOG_WARN SPDLOG_WARN
#define DEBUG_LOG_ERROR SPDLOG_ERROR

namespace LitchiRuntime
{
	/**
	 * @brief Some log levels
	*/
	enum class ELogLevel
	{
		LOG_DEFAULT,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR
	};

	/**
	 * @brief Store the log information
	*/
	struct LogData
	{
		std::string message;
		ELogLevel logLevel;
		std::string date;
	};

    /**
     * @brief Manage Debug
    */
    class Debug {
    public:
        static void Initialize();
		static Event<const LogData&> LogEvent;
    };
}


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
	* Some log modes (Indicates the handler to use)
	*/
	enum class ELogMode
	{
		DEFAULT,
		CONSOLE,
		FILE,
		ALL
	};

    class Debug {
    public:
        static void Initialize();
		static Event<ELogMode, const std::string&> LogEvent;
    };
}

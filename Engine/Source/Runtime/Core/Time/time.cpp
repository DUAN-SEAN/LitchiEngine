
#include "Runtime/Core/pch.h"
#include "time.h"
namespace LitchiRuntime
{
    std::chrono::system_clock::time_point Time::s_startupTime;
    float Time::m_deltaTime = 0;
    float Time::s_lastFrameTime = 0;
    float Time::m_fixedUpdateTime = 1.0 / 60;

    Time::Time() {
    }

    Time::~Time() {
    }

    void Time::Initialize() {
        s_startupTime = std::chrono::system_clock::now();
    }

    void Time::Update() {
        if (s_lastFrameTime > 0) {
            m_deltaTime = TimeSinceStartup() - s_lastFrameTime;
        }
        s_lastFrameTime = TimeSinceStartup();
    }

    float Time::TimeSinceStartup() {
        std::chrono::time_point now = std::chrono::system_clock::now();
        uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - s_startupTime).count();
        return ms / 1000.0f;
    }
}

#pragma once

//= INCLUDES ===========
#include <chrono>
#include "Runtime/Core/Definitions.h"
//======================

namespace LitchiRuntime
{
    class LC_CLASS Stopwatch
    {
    public:
        Stopwatch() { Start(); }
        ~Stopwatch() = default;

        void Start()
        {
            m_start = std::chrono::high_resolution_clock::now();
        }

        float GetElapsedTimeSec() const
        {
            const std::chrono::duration<double, std::milli> ms = std::chrono::high_resolution_clock::now() - m_start;
            return static_cast<float>(ms.count() / 1000);
        }

        float GetElapsedTimeMs() const
        {
            const std::chrono::duration<double, std::milli> ms = std::chrono::high_resolution_clock::now() - m_start;
            return static_cast<float>(ms.count());
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };
}

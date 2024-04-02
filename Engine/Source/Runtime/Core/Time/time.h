#pragma once

#include <string>
#include <chrono>
namespace LitchiRuntime
{
    /**
     * @brief Manage engine running time
    */
    class Time
    {
    public:

        /**
         * @brief Construct
        */
        Time();
        ~Time();

        /**
         * @brief Initialize time
        */
        static void Initialize();

        /**
         * @brief Update running time
        */
        static void Update();

        /**
         * @brief Get the game running time
         * @return 
        */
        static float TimeSinceStartup();

        /**
         * @brief Get time spent on the last frame
         * this is render interval time
         * @return 
        */
        static float GetDeltaTime() { return m_deltaTime; }

        /**
         * @brief Get fixed update time
         * @return fixed time 
        */
        static float GetFixedUpdateTime() { return m_fixedUpdateTime; }

        /**
         * @brief Set fixed update time
         * @param time 
        */
        static void SetFixedUpdateTime(float time) { m_fixedUpdateTime = time; }

    private:

        /**
         * @brief engine start time
        */
        static std::chrono::system_clock::time_point s_startupTime;

        /**
         * @brief last frame time
        */
        static float s_lastFrameTime;

        /**
         * @brief The time spent on the last frame
        */
        static float m_deltaTime;

        /**
         * @brief Fixed update time
         * usually used for physics simulation
        */
        static float m_fixedUpdateTime;
    };
}

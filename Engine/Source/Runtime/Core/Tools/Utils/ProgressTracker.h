
#pragma once

//= INCLUDES ===================
#include <atomic>
#include <string>
#include "Runtime//Core/Definitions.h"
//==============================

namespace LitchiRuntime
{
    enum class ProgressType
    {
        ModelImporter,
        World,
        Resource,
        Terrain,
        undefined
    };

    class LC_CLASS Progress
    {
    public:
        void Start(const uint32_t job_count, const std::string& text);

        float GetFraction();
        void JobDone();

        const std::string& GetText();
        void SetText(const std::string& text);

        bool IsProgressing();

    private:
        std::atomic<uint32_t> m_jobs_done = 0;
        std::atomic<uint32_t> m_job_count = 0;
        std::string m_text;
    };

    class LC_CLASS ProgressTracker
    {
    public:
        static Progress& GetProgress(const ProgressType progress_type);
    };
}

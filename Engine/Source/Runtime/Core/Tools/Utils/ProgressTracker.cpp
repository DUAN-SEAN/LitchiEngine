
//= INCLUDES ===============
#include "Runtime/Core/pch.h"
#include "ProgressTracker.h"
//==========================

//= NAMESPACES ===============
using namespace std;
//============================

namespace LitchiRuntime
{
    static std::array<Progress, 4> m_progresses;
    static std::mutex m_mutex_progress_access;
    static std::mutex m_mutex_jobs;

    void Progress::Start(const uint32_t job_count, const std::string& text)
    {
        LC_ASSERT_MSG(GetFraction() == 1.0f, "The previous progress tracking hasn't finished");

        lock_guard lock(m_mutex_jobs);

        m_job_count = job_count;
        m_jobs_done = 0;
        m_text      = text;
    }

    float Progress::GetFraction()
    {
        lock_guard lock(m_mutex_jobs);

        if (m_job_count == 0)
            return 1.0f;

        return static_cast<float>(m_jobs_done) / static_cast<float>(m_job_count);
    }

    bool Progress::IsProgressing()
    {
        return GetFraction() != 1.0f;
    }

    void Progress::JobDone()
    {
        lock_guard lock(m_mutex_jobs);

        LC_ASSERT_MSG(m_jobs_done + 1 <= m_job_count, "Job count exceeded");
        m_jobs_done++;
    }

	const string& Progress::GetText()
    {
        return m_text;
    }

    void Progress::SetText(const string& text)
    {
        m_text = text;
    }

    Progress& ProgressTracker::GetProgress(const ProgressType progress_type)
    {
        lock_guard lock(m_mutex_progress_access);
        return m_progresses[static_cast<uint32_t>(progress_type)];
    }
}

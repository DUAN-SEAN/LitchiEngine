
#include "Editor/include/Panels/Profiler.h"

#include <Runtime/Function/UI/Widgets/Visual/Separator.h>

#include "Runtime/Core/Log/debug.h"

namespace LitchiEditor
{

	Profiler::Profiler
	(
		const std::string& p_title,
		bool p_opened,
		const LitchiRuntime::PanelWindowSettings& p_windowSettings,
		float p_frequency
	) :
		PanelWindow(p_title, p_opened, p_windowSettings),
		m_frequency(p_frequency)
	{
		allowHorizontalScrollbar = true;

		CreateWidget<LitchiRuntime::Text>("Profiler state: ").lineBreak = false;
		CreateWidget<LitchiRuntime::CheckBox>(false, "").ValueChangedEvent += std::bind(&Profiler::Enable, this, std::placeholders::_1, false);

		m_fpsText = &CreateWidget<LitchiRuntime::TextColored>("");
		m_captureResumeButton = &CreateWidget<LitchiRuntime::Button>("Capture");
		m_captureResumeButton->idleBackgroundColor = { 0.7f, 0.5f, 0.f };
		m_captureResumeButton->ClickedEvent += [this]
			{
				m_profilingMode = m_profilingMode == EProfilingMode::CAPTURE ? EProfilingMode::DEFAULT : EProfilingMode::CAPTURE;
				m_captureResumeButton->label = m_profilingMode == EProfilingMode::CAPTURE ? "Resume" : "Capture";
			};
		m_elapsedFramesText = &CreateWidget<LitchiRuntime::TextColored>("", LitchiRuntime::Color(1.f, 0.8f, 0.01f, 1));
		m_elapsedTimeText = &CreateWidget<LitchiRuntime::TextColored>("", LitchiRuntime::Color(1.f, 0.8f, 0.01f, 1));
		m_separator = &CreateWidget<LitchiRuntime::Separator>();
		m_actionList = &CreateWidget<LitchiRuntime::Columns<5>>();
		m_actionList->widths = { 300.f, 100.f, 100.f, 100.f, 200.f };

		Enable(false, true);
	}

	void Profiler::Update(float p_deltaTime)
	{
		m_timer += p_deltaTime;
		m_fpsTimer += p_deltaTime;

		while (m_fpsTimer >= 0.07f)
		{
			m_fpsText->content = "FPS: " + std::to_string(static_cast<int>(1.0f / p_deltaTime));
			m_fpsTimer -= 0.07f;
		}

		//if (m_profiler.IsEnabled())
		//{
		//	m_profiler.Update(p_deltaTime);

		//	while (m_timer >= m_frequency)
		//	{
		//		if (m_profilingMode == EProfilingMode::DEFAULT)
		//		{
		//			OvAnalytics::Profiling::ProfilerReport report = m_profiler.GenerateReport();
		//			m_profiler.ClearHistory();
		//			m_actionList->RemoveAllWidgets();

		//			m_elapsedFramesText->content = "Elapsed frames: " + std::to_string(report.elapsedFrames);
		//			m_elapsedTimeText->content = "Elapsed time: " + std::to_string(report.elaspedTime);

		//			m_actionList->CreateWidget<LitchiRuntime::Text>("Action");
		//			m_actionList->CreateWidget<LitchiRuntime::Text>("Total duration");
		//			m_actionList->CreateWidget<LitchiRuntime::Text>("Frame duration");
		//			m_actionList->CreateWidget<LitchiRuntime::Text>("Frame load");
		//			m_actionList->CreateWidget<LitchiRuntime::Text>("Total calls");

		//			for (auto& action : report.actions)
		//			{
		//				auto color = CalculateActionColor(action.percentage);
		//				m_actionList->CreateWidget<LitchiRuntime::TextColored>(action.name, color);
		//				m_actionList->CreateWidget<LitchiRuntime::TextColored>(std::to_string(action.duration) + "s", color);
		//				m_actionList->CreateWidget<LitchiRuntime::TextColored>(std::to_string(action.duration / action.calls) + "s", color);
		//				m_actionList->CreateWidget<LitchiRuntime::TextColored>(std::to_string(action.percentage) + "%%", color);
		//				m_actionList->CreateWidget<LitchiRuntime::TextColored>(std::to_string(action.calls) + " calls", color);
		//			}
		//		}

		//		m_timer -= m_frequency;
		//	}
		//}
	}

	void Profiler::Enable(bool p_value, bool p_disableLog)
	{
		/*if (p_value)
		{
			if (!p_disableLog)
				DEBUG_LOG_INFO("Profiling started!");
			m_profiler.Enable();
		}
		else
		{
			if (!p_disableLog)
				DEBUG_LOG_INFO("Profiling stoped!");
			m_profiler.Disable();
			m_profiler.ClearHistory();
			m_actionList->RemoveAllWidgets();
		}

		m_captureResumeButton->enabled = p_value;
		m_elapsedFramesText->enabled = p_value;
		m_elapsedTimeText->enabled = p_value;
		m_separator->enabled = p_value;*/
	}

	LitchiRuntime::Color Profiler::CalculateActionColor(double p_percentage) const
	{
		if (p_percentage <= 25.0f)		return { 0.0f, 1.0f, 0.0f, 1.0f };
		else if (p_percentage <= 50.0f) return { 1.0f, 1.0f, 0.0f, 1.0f };
		else if (p_percentage <= 75.0f) return { 1.0f, 0.6f, 0.0f, 1.0f };
		else							return { 1.0f, 0.0f, 0.0f, 1.0f };
	}

	/*std::string Profiler::GenerateActionString(OvAnalytics::Profiling::ProfilerReport::Action& p_action)
	{
		std::string result;

		result += "[" + p_action.name + "]";
		result += std::to_string(p_action.duration) + "s (total) | ";
		result += std::to_string(p_action.duration / p_action.calls) + "s (per call) | ";
		result += std::to_string(p_action.percentage) + "%% | ";
		result += std::to_string(p_action.calls) + " calls";

		return result;
	}*/
}

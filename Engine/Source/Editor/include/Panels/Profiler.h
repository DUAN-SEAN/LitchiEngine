
#pragma once

//#include <OvAnalytics/Profiling/Profiler.h>

#include <Runtime/Function/UI/Panels/PanelWindow.h>
#include <Runtime/Function/UI/Widgets/Texts/TextColored.h>
#include <Runtime/Function/UI/Widgets/Selection/CheckBox.h>
#include <Runtime/Function/UI/Widgets/Layout/Group.h>
#include <Runtime/Function/UI/Widgets/Layout/Columns.h>
#include <Runtime/Function/UI/Widgets/Buttons/Button.h>

namespace LitchiEditor
{
	class Profiler : public LitchiRuntime::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		* @param p_frequency
		*/
		Profiler
		(
			const std::string& p_title,
			bool p_opened,
			const LitchiRuntime::PanelWindowSettings& p_windowSettings,
			float p_frequency
		);

		/**
		* Update profiling information
		* @param p_deltaTime
		*/
		void Update(float p_deltaTime);

		/**
		* Enable or disable the profiler
		* @param p_value
		* @param p_disableLog
		*/
		void Enable(bool p_value, bool p_disableLog = false);

	private:
		LitchiRuntime::Color CalculateActionColor(double p_percentage) const;
		// std::string GenerateActionString(OvAnalytics::Profiling::ProfilerReport::Action& p_action);

	private:
		enum class EProfilingMode
		{
			DEFAULT,
			CAPTURE
		};

		float m_frequency;
		float m_timer = 0.f;
		float m_fpsTimer = 0.f;
		EProfilingMode m_profilingMode = EProfilingMode::DEFAULT;

		//OvAnalytics::Profiling::Profiler m_profiler;

		LitchiRuntime::AWidget* m_separator;
		LitchiRuntime::Button* m_captureResumeButton;
		LitchiRuntime::TextColored* m_fpsText;
		LitchiRuntime::TextColored* m_elapsedFramesText;
		LitchiRuntime::TextColored* m_elapsedTimeText;
		LitchiRuntime::Columns<5>* m_actionList;
	};
}
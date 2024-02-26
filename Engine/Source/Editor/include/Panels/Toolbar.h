
#pragma once

#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Function/UI/Widgets/Buttons/ButtonImage.h"

namespace LitchiEditor
{
	class Toolbar : public LitchiRuntime::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Toolbar
		(
			const std::string& p_title,
			bool p_opened,
			const LitchiRuntime::PanelWindowSettings& p_windowSettings
		);

		/**
		* Custom implementation of the draw method
		*/
		void _Draw_Impl() override;

	private:
		/*LitchiRuntime::ButtonImage* m_playButton;
		LitchiRuntime::ButtonImage* m_pauseButton;
		LitchiRuntime::ButtonImage* m_stopButton;
		LitchiRuntime::ButtonImage* m_nextButton;*/

		LitchiRuntime::Button* m_playButton;
		LitchiRuntime::Button* m_pauseButton;
		LitchiRuntime::Button* m_stopButton;
		LitchiRuntime::Button* m_nextButton;
	};
}

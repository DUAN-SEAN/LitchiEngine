
#include <Runtime/Function/UI/Widgets/Layout/Spacing.h>

#include "Editor/include/Panels/Toolbar.h"
#include "Editor/include/Core/EditorActions.h"

#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Resource/TextureManager.h"

namespace LitchiEditor
{
	Toolbar::Toolbar
(
	const std::string& p_title,
	bool p_opened,
	const LitchiRuntime::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	std::string iconFolder = ":Textures/Icons/";

	// auto& textureManager = LitchiRuntime::ServiceLocator::Get<LitchiRuntime::TextureManager>();
	//m_playButton	= &CreateWidget<LitchiRuntime::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Button_Play")->id, LitchiRuntime::Vector2 { 20, 20 });
	//m_pauseButton	= &CreateWidget<LitchiRuntime::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Button_Pause")->id, LitchiRuntime::Vector2{ 20, 20 });
	//m_stopButton	= &CreateWidget<LitchiRuntime::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Button_Stop")->id, LitchiRuntime::Vector2{ 20, 20 });
	//m_nextButton	= &CreateWidget<LitchiRuntime::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Button_Next")->id, LitchiRuntime::Vector2{ 20, 20 });

	//CreateWidget<LitchiRuntime::Spacing>(0).lineBreak = false;
	//auto& refreshButton	= CreateWidget<LitchiRuntime::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Button_Refresh")->id, LitchiRuntime::Vector2{ 20, 20 });

	m_playButton = &CreateWidget<LitchiRuntime::Button>("Play", LitchiRuntime::Vector2 { 100, 20 });
	m_pauseButton = &CreateWidget<LitchiRuntime::Button>("Pause", LitchiRuntime::Vector2 { 100, 20 });
	m_stopButton = &CreateWidget<LitchiRuntime::Button>("Stop", LitchiRuntime::Vector2 { 100, 20 });
	m_nextButton = &CreateWidget<LitchiRuntime::Button>("Next", LitchiRuntime::Vector2 { 100, 20 });

	CreateWidget<LitchiRuntime::Spacing>(0).lineBreak = false;
	auto& refreshButton = CreateWidget<LitchiRuntime::Button>("Refresh", LitchiRuntime::Vector2 { 100, 20 });

	m_playButton->lineBreak		= false;
	m_pauseButton->lineBreak	= false;
	m_stopButton->lineBreak		= false;
	m_nextButton->lineBreak		= false;
	refreshButton.lineBreak		= false;

	m_playButton->ClickedEvent	+= EDITOR_BIND(StartPlaying);
	m_pauseButton->ClickedEvent	+= EDITOR_BIND(PauseGame);
	m_stopButton->ClickedEvent	+= EDITOR_BIND(StopPlaying);
	m_nextButton->ClickedEvent	+= EDITOR_BIND(NextFrame);
	refreshButton.ClickedEvent	+= EDITOR_BIND(RefreshScripts);

	EDITOR_EVENT(EditorModeChangedEvent) += [this](EditorActions::EEditorMode p_newMode)
	{
		//auto enable = [](LitchiRuntime::ButtonImage* p_button, bool p_enable)
		const auto enable = [](LitchiRuntime::Button* p_button, bool p_enable)
		{
			p_button->disabled = !p_enable;
			// p_button->tint = p_enable ? LitchiRuntime::Color{ 1.0f, 1.0f, 1.0f, 1.0f} : LitchiRuntime::Color{1.0f, 1.0f, 1.0f, 0.15f};
			p_button->textColor = p_enable ? LitchiRuntime::Color{ 1.0f, 1.0f, 1.0f, 1.0f} : LitchiRuntime::Color{1.0f, 1.0f, 1.0f, 0.15f};
		};

		switch (p_newMode)
		{
		case EditorActions::EEditorMode::EDIT:
			enable(m_playButton, true);
			enable(m_pauseButton, false);
			enable(m_stopButton, false);
			enable(m_nextButton, false);
			break;
		case EditorActions::EEditorMode::PLAY:
			enable(m_playButton, false);
			enable(m_pauseButton, true);
			enable(m_stopButton, true);
			enable(m_nextButton, true);
			break;
		case EditorActions::EEditorMode::PAUSE:
			enable(m_playButton, true);
			enable(m_pauseButton, false);
			enable(m_stopButton, true);
			enable(m_nextButton, true);
			break;
		case EditorActions::EEditorMode::FRAME_BY_FRAME:
			enable(m_playButton, true);
			enable(m_pauseButton, false);
			enable(m_stopButton, true);
			enable(m_nextButton, true);
			break;
		}
	};

	EDITOR_EXEC(SetEditorMode(EditorActions::EEditorMode::EDIT));
}

void Toolbar::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

	PanelWindow::_Draw_Impl();

	ImGui::PopStyleVar();
}

}


#include "Runtime/Core/pch.h"

#include "Editor/include/Panels/Console.h"
#include "Editor/include/Core/EditorActions.h"

#include <Runtime/Function/UI/Widgets/Buttons/Button.h>
#include <Runtime/Function/UI/Widgets/Selection/CheckBox.h>
#include <Runtime/Function/UI/Widgets/Visual/Separator.h>
#include <Runtime/Function/UI/Widgets/Layout/Spacing.h>

#include "Runtime/Core/Global/ServiceLocator.h"

namespace LitchiEditor
{
std::pair<LitchiRuntime::Color, std::string> GetWidgetSettingsFromLogData(const LitchiRuntime::LogData& p_logData)
{
	LitchiRuntime::Color logColor;
	std::string logHeader;

	switch (p_logData.logLevel)
	{
	default:
	case LitchiRuntime::ELogLevel::LOG_DEFAULT:	return { { 1.f, 1.f, 1.f, 1.f }, p_logData.date };
	case LitchiRuntime::ELogLevel::LOG_INFO:		return { { 0.f, 1.f, 1.f, 1.f }, p_logData.date };
	case LitchiRuntime::ELogLevel::LOG_WARNING:	return { { 1.f, 1.f, 0.f, 1.f }, p_logData.date };
	case LitchiRuntime::ELogLevel::LOG_ERROR:		return { { 1.f, 0.f, 0.f, 1.f }, p_logData.date };
	}
}

LitchiEditor::Console::Console
(
	const std::string& p_title,
	bool p_opened,
	const LitchiRuntime::PanelWindowSettings& p_windowSettings
) :
	PanelWindow(p_title, p_opened, p_windowSettings)
{
	allowHorizontalScrollbar = true;

	auto& clearButton = CreateWidget<LitchiRuntime::Button>("Clear");
	clearButton.size = { 50.f, 0.f };
	clearButton.idleBackgroundColor = { 0.5f, 0.f, 0.f };
	clearButton.ClickedEvent += std::bind(&Console::Clear, this);
	clearButton.lineBreak = false;

	auto& clearOnPlay = CreateWidget<LitchiRuntime::CheckBox>(m_clearOnPlay, "Auto clear on play");

	CreateWidget<LitchiRuntime::Spacing>(5).lineBreak = false;

	auto& enableDefault = CreateWidget<LitchiRuntime::CheckBox>(true, "Default");
	auto& enableInfo = CreateWidget<LitchiRuntime::CheckBox>(true, "Info");
	auto& enableWarning = CreateWidget<LitchiRuntime::CheckBox>(true, "Warning");
	auto& enableError = CreateWidget<LitchiRuntime::CheckBox>(true, "Error");

	clearOnPlay.lineBreak = false;
	enableDefault.lineBreak = false;
	enableInfo.lineBreak = false;
	enableWarning.lineBreak = false;
	enableError.lineBreak = true;

	clearOnPlay.ValueChangedEvent += [this](bool p_value) { m_clearOnPlay = p_value; };
	enableDefault.ValueChangedEvent += std::bind(&Console::SetShowDefaultLogs, this, std::placeholders::_1);
	enableInfo.ValueChangedEvent += std::bind(&Console::SetShowInfoLogs, this, std::placeholders::_1);
	enableWarning.ValueChangedEvent += std::bind(&Console::SetShowWarningLogs, this, std::placeholders::_1);
	enableError.ValueChangedEvent += std::bind(&Console::SetShowErrorLogs, this, std::placeholders::_1);

	CreateWidget<LitchiRuntime::Separator>();

	m_logGroup = &CreateWidget<LitchiRuntime::Group>();
    m_logGroup->ReverseDrawOrder();

	EDITOR_EVENT(PlayEvent) += std::bind(&Console::ClearOnPlay, this);

	LitchiRuntime::Debug::LogEvent += std::bind(&Console::OnLogIntercepted, this, std::placeholders::_1);
}

void Console::OnLogIntercepted(const LitchiRuntime::LogData & p_logData)
{
	auto[logColor, logDate] = GetWidgetSettingsFromLogData(p_logData);

	auto& consoleItem1 = m_logGroup->CreateWidget<LitchiRuntime::TextColored>(logDate + "\t" + p_logData.message, logColor);

	consoleItem1.enabled = IsAllowedByFilter(p_logData.logLevel);

	m_logTextWidgets[&consoleItem1] = p_logData.logLevel;
}

void Console::ClearOnPlay()
{
	if (m_clearOnPlay)
		Clear();
}

void Console::Clear()
{
	m_logTextWidgets.clear();
	m_logGroup->RemoveAllWidgets();
}

void Console::FilterLogs()
{
	for (const auto&[widget, logLevel] : m_logTextWidgets)
		widget->enabled = IsAllowedByFilter(logLevel);
}

bool Console::IsAllowedByFilter(LitchiRuntime::ELogLevel p_logLevel)
{
	switch (p_logLevel)
	{
	case LitchiRuntime::ELogLevel::LOG_DEFAULT:	return m_showDefaultLog;
	case LitchiRuntime::ELogLevel::LOG_INFO:		return m_showInfoLog;
	case LitchiRuntime::ELogLevel::LOG_WARNING:	return m_showWarningLog;
	case LitchiRuntime::ELogLevel::LOG_ERROR:		return m_showErrorLog;
	}

	return false;
}

void Console::SetShowDefaultLogs(bool p_value)
{
	m_showDefaultLog = p_value;
	FilterLogs();
}

void Console::SetShowInfoLogs(bool p_value)
{
	m_showInfoLog = p_value;
	FilterLogs();
}

void Console::SetShowWarningLogs(bool p_value)
{
	m_showWarningLog = p_value;
	FilterLogs();
}

void LitchiEditor::Console::SetShowErrorLogs(bool p_value)
{
	m_showErrorLog = p_value;
	FilterLogs();
}
}


#include "PathParser.h"
#include "SystemCalls.h"

#include <Windows.h>

void LitchiRuntime::SystemCalls::ShowInExplorer(const std::string & p_path)
{
	ShellExecuteA(NULL, "open", LitchiRuntime::PathParser::MakeWindowsStyle(p_path).c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void LitchiRuntime::SystemCalls::OpenFile(const std::string & p_file, const std::string & p_workingDir)
{
	ShellExecuteA(NULL, "open", LitchiRuntime::PathParser::MakeWindowsStyle(p_file).c_str(), NULL,
		p_workingDir.empty() ? NULL : LitchiRuntime::PathParser::MakeWindowsStyle(p_workingDir).c_str(),
		SW_SHOWNORMAL);
}

void LitchiRuntime::SystemCalls::EditFile(const std::string & p_file)
{
	ShellExecuteW(NULL, NULL, std::wstring(p_file.begin(), p_file.end()).c_str(), NULL, NULL, SW_NORMAL);
}

void LitchiRuntime::SystemCalls::OpenURL(const std::string& p_url)
{
	ShellExecute(0, 0, p_url.c_str(), 0, 0, SW_SHOW);
}

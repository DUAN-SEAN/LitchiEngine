
#include "Runtime/Core/pch.h"

#include "OpenFileDialog.h"

#include <Windows.h>

LitchiRuntime::OpenFileDialog::OpenFileDialog(const std::string & p_dialogTitle) : FileDialog(GetOpenFileNameA, p_dialogTitle)
{
}

void LitchiRuntime::OpenFileDialog::AddFileType(const std::string & p_label, const std::string & p_filter)
{
	m_filter += p_label + '\0' + p_filter + '\0';
}

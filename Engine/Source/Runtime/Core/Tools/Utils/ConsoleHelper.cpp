
#include "Runtime/Core/pch.h"
#include "ConsoleHelper.h"
#include <Windows.h>

namespace LitchiRuntime
{
    void ConsoleHelper::HideConsole()
    {
        ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    }

    void ConsoleHelper::ShowConsole()
    {
        ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    }

    bool ConsoleHelper::IsConsoleVisible()
    {
        return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
    }
	
}

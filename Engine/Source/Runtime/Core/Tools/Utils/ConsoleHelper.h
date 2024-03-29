
#pragma once

namespace LitchiRuntime
{
	class ConsoleHelper
	{
	public:
		/**
		* Disabled constructor
		*/
		ConsoleHelper() = delete;

		static void HideConsole();

		static void ShowConsole();

		static bool IsConsoleVisible();
	};
}
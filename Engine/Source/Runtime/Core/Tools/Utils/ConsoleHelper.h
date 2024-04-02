
#pragma once

namespace LitchiRuntime
{
	/**
	 * @brief Console Helper control console window
	*/
	class ConsoleHelper
	{
	public:
		
		/**
		 * @brief Disabled constructor
		*/
		ConsoleHelper() = delete;

		/**
		 * @brief Hide console window
		*/
		static void HideConsole();

		/**
		 * @brief Show console window
		*/
		static void ShowConsole();

		/**
		 * @brief console is visible
		 * @return 
		*/
		static bool IsConsoleVisible();
	};
}
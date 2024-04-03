
#pragma once

#include <string>

#include "Runtime/Core/Window/Cursor/ECursorMode.h"
#include "Runtime/Core/Window/Cursor/ECursorShape.h"

namespace LitchiRuntime
{
	/**
	 * @brief Contains window settings
	 */
	struct WindowSettings 
	{
		/**
		 * @brief A simple constant used to ignore a value setting (Let the program decide for you)
		 * @note You can you WindowSettings::DontCare only where it is indicated
		 */
		static const int32_t DontCare = -1;

		/**
		 * @brief Title of the window (Displayed in the title bar)
		 */
		std::string title;

		/**
		 * @brief Width in pixels of the window
		 */
		uint16_t width;

		/**
		 * @brief Height in pixels of the window
		 */
		uint16_t height;

		/**
		 * @brief Minimum width of the window.
		 * Use WindowSettings::DontCare to disable limit
		 */
		int16_t minimumWidth = DontCare;

		/**
		 * @brief Minimum height of the window.
		 * Use WindowSettings::DontCare to disable limit
		 */
		int16_t minimumHeight = DontCare;

		/**
		 * @brief Maximum width of the window.
		 * Use WindowSettings::DontCare to disable limit
		 */
		int16_t maximumWidth = DontCare;

		/**
		 * @brief Maximum height of the window.
		 * Use WindowSettings::DontCare to disable limit
		 */
		int16_t maximumHeight = DontCare;

		/**
		 * @brief Specifies if the window is by default in fullscreen or windowed mode
		 */
		bool fullscreen = false;

		/**
		 * @brief Specifies whether the windowed mode window will have window decorations such as a border, a close widget, etc.
		 * An undecorated window may still allow the user to generate close events on some platforms. This hint is ignored
		 * for full screen windows.
		 */
		bool decorated = true;

		/**
		 * @brief specifies whether the windowed mode window will be resizable by the user. The window will still be resizable using
		 * the "SetSize(uint16_t, uint16_t)" method of the "Window" class. This hint is ignored for full screen windows
		 */
		bool resizable = true;

		/**
		 * @brief Specifies whether the windowed mode window will be given input focus when created. This hint is ignored for
		 * full screen and initially hidden windows.
		 */
		bool focused = true;

		/**
		 * @brief Specifies whether the windowed mode window will be maximized when created. This hint is ignored for full screen windows.
		 */
		bool maximized = false;

		/**
		 * @brief Specifies whether the windowed mode window will be floating above other regular windows, also called topmost or always-on-top.
		 * This is intended primarily for debugging purposes and cannot be used to implement proper full screen windows. This hint is
		 * ignored for full screen windows.
		 */
		bool floating = false;

		/**
		 * @brief Specifies whether the windowed mode window will be initially visible. This hint is ignored for full screen windows.
		 */
		bool visible = true;

		/**
		 * @brief Specifies whether the full screen window will automatically iconify and restore
		 * the previous video mode on input focus loss. This hint is ignored for windowed mode windows
		 */
		bool autoIconify = true;

		/**
		 * @brief Specifies the desired refresh rate for full screen windows. If set to WindowSettings::DontCare, the highest
		 * available refresh rate will be used. This hint is ignored for windowed mode windows.
		 */
		int32_t refreshRate = WindowSettings::DontCare;

		/**
		 * @brief Specifies the default cursor mode of the window
		 */
		ECursorMode cursorMode = ECursorMode::NORMAL;

		/**
		 * @brief Specifies the default cursor shape of the window
		 */
		ECursorShape cursorShape = ECursorShape::ARROW;

		/**
		 * @brief Defines the number of samples to use (For anti-aliasing)
		 */
		uint32_t samples = 4;
	};
}

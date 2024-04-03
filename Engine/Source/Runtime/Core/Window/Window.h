

#pragma once

#include <string>
#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Settings/WindowSettings.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace LitchiRuntime
{
	/**
	 * @brief Some errors that the driver can return
	 */
	enum class EWindowError
	{
		NOT_INITIALIZED = 0x00010001,
		NO_CURRENT_CONTEXT = 0x00010002,
		INVALID_ENUM = 0x00010003,
		INVALID_VALUE = 0x00010004,
		OUT_OF_MEMORY = 0x00010005,
		API_UNAVAILABLE = 0x00010006,
		VERSION_UNAVAILABLE = 0x00010007,
		PLATFORM_ERROR = 0x00010008,
		FORMAT_UNAVAILABLE = 0x00010009,
		NO_WINDOW_CONTEXT = 0x0001000A
	};

	/**
	 * @brief A simple OS-based window. It needs a Device (GLFW) to work
	 */
	class Window
	{
	public:

		/**
		 * @brief Create the window
		 * @param p_windowSettings 
		 */
		Window(const WindowSettings& p_windowSettings);

		/**
		 * @brief Destructor of the window, responsible of the GLFW window memory free
		 */
		~Window();

		/**
		 * @brief Set Icon
		 * @param p_filePath 
		 */
		void SetIcon(const std::string& p_filePath) const;

		/**
		 * @brief Set Icon from memory
		 * @param p_data 
		 * @param p_width 
		 * @param p_height 
		 */
		void SetIconFromMemory(uint8_t* p_data, uint32_t p_width, uint32_t p_height);

		/**
		 * @brief Find an instance of window with a given GLFWindow
		 * @param p_glfwWindow 
		 * @return 
		 */
		static Window* FindInstance(GLFWwindow* p_glfwWindow);

		/**
		 * @brief Resize the window
		 * @param p_width 
		 * @param p_height 
		 */
		void SetSize(uint16_t p_width, uint16_t p_height);

		/**
		 * @brief Defines a minimum size for the window
		 * @param p_minimumWidth 
		 * @param p_minimumHeight
		 * @note -1 (WindowSettings::DontCare) value means no limitation
		 */
		void SetMinimumSize(int16_t p_minimumWidth, int16_t p_minimumHeight);

		/**
		 * @brief Defines a maximum size for the window
		 * @param p_maximumWidth 
		 * @param p_maximumHeight
		 * @note -1 (WindowSettings::DontCare) value means no limitation
		 */
		void SetMaximumSize(int16_t p_maximumWidth, int16_t p_maximumHeight);

		/**
		 * @brief Define a position for the window
		 * @param p_x 
		 * @param p_y 
		 */
		void SetPosition(int16_t p_x, int16_t p_y);

		/**
		 * @brief Minimize the window
		 */
		void Minimize() const;

		/**
		 * @brief Maximize the window
		 */
		void Maximize() const;

		/**
		 * @brief Restore the window
		 */
		void Restore() const;

		/**
		 * @brief Hides the specified window if it was previously visible
		 */
		void Hide() const;

		/**
		 * @brief Show the specified window if it was previously hidden
		 */
		void Show() const;

		/**
		 * @brief Focus the window
		 */
		void Focus() const;

		/**
		 * @brief Set the should close flag of the window to true
		 * @param p_value 
		 */
		void SetShouldClose(bool p_value) const;

		/**
		 * @brief Return true if the window should close
		 * @return 
		 */
		bool ShouldClose() const;

		/**
		 * @brief Set the window in fullscreen or windowed mode
		 * @param p_value (True for fullscreen mode, false for windowed)
		 */
		void SetFullscreen(bool p_value);

		/**
		 * @brief Switch the window to fullscreen or windowed mode depending on the current state
		 */
		void ToggleFullscreen();

		/**
		 * @brief Return true if the window is fullscreen
		 * @return 
		 */
		bool IsFullscreen() const;

		/**
		 * @brief Return true if the window is hidden
		 * @return 
		 */
		bool IsHidden() const;

		/**
		 * @brief Return true if the window is visible
		 * @return 
		 */
		bool IsVisible() const;

		/**
		 * @brief Return true if the windows is maximized
		 * @return 
		 */
		bool IsMaximized() const;

		/**
		 * @brief Return true if the windows is minimized
		 * @return 
		 */
		bool IsMinimized() const;

		/**
		 * @brief Return true if the windows is focused
		 * @return 
		 */
		bool IsFocused() const;

		/**
		 * @brief Return true if the windows is resizable
		 * @return 
		 */
		bool IsResizable() const;

		/**
		 * @brief Return true if the windows is decorated
		 * @return 
		 */
		bool IsDecorated() const;

		/**
		 * @brief Define the window as the current context
		 */
		void MakeCurrentContext() const;

		/**
		 * @brief Handle the buffer swapping with the current window
		 */
		void SwapBuffers() const;

		/**
		 * @brief Define a mode for the mouse cursor
		 * @param p_cursorMode 
		 */
		void SetCursorMode(ECursorMode p_cursorMode);

		/**
		 * @brief Define a shape to apply to the current cursor
		 * @param p_cursorShape 
		 */
		void SetCursorShape(ECursorShape p_cursorShape);

		/**
		 * @brief Move the cursor to the given position
		 * @param p_x 
		 * @param p_y 
		 */
		void SetCursorPosition(int16_t p_x, int16_t p_y);

		/**
		 * @brief Define a title for the window
		 * @param p_title 
		 */
		void SetTitle(const std::string& p_title);

		/**
		 * @brief Defines a refresh rate (Use WindowSettings::DontCare to use the highest available refresh rate)
		 * @param p_refreshRate You need to switch to fullscreen mode to apply this effect (Or leave fullscreen and re-apply)
		 */
		void SetRefreshRate(int32_t p_refreshRate);

		/**
		 * @brief Return the title of the window
		 * @return 
		 */
		std::string GetTitle() const;

		/**
		 * @brief Get the window width 
		 * @return 
		 */
		uint32_t GetWidth();

		/**
		 * @brief Get the window height
		 * @return
		 */
		uint32_t GetHeight();

		/**
		 * @brief Return the current size of the window
		 * @return 
		 */
		std::pair<uint16_t, uint16_t> GetSize() const;

		/**
		 * @brief Return the current minimum size of the window
		 * @return
		 * @note -1 (WindowSettings::DontCare) values means no limitation
		 */
		std::pair<int16_t, int16_t> GetMinimumSize() const;

		/**
		 * @brief Return the current maximum size of the window
		 * @return
		 * @note -1 (WindowSettings::DontCare) values means no limitation
		 */
		std::pair<int16_t, int16_t> GetMaximumSize() const;

		/**
		 * @brief Return the current position of the window
		 * @return 
		 */
		std::pair<int16_t, int16_t> GetPosition() const;

		/**
		 * @brief Return the framebuffer size (Viewport size)
		 * @return 
		 */
		std::pair<uint16_t, uint16_t> GetFramebufferSize() const;

		/**
		 * @brief Return the current cursor mode
		 * @return 
		 */
		ECursorMode GetCursorMode() const;

		/**
		 * @brief Return the current cursor shape
		 * @return 
		 */
		ECursorShape GetCursorShape() const;

		/**
		 * @brief Return the current refresh rate (Only applied to the fullscreen mode).
		 * If the value is -1 (WindowSettings::DontCare) the highest refresh rate will be used
		 * @return 
		 */
		int32_t GetRefreshRate() const;

		/**
		 * @brief Return GLFW window
		 * @return 
		 */
		GLFWwindow* GetGlfwWindow() const;

		/**
		 * @brief Get dpi scale
		 * @return 
		 */
		float GetDpiScale();

		/**
		 * @brief Return the size, in pixels, of the primary monity
		 * @return 
		 */
		std::pair<int16_t, int16_t> GetMonitorSize() const;

		/**
		 * @brief Return an instance of GLFWcursor corresponding to the given shape
		 * @param p_cursorShape 
		 * @return 
		 */
		GLFWcursor* GetCursorInstance(ECursorShape p_cursorShape) const;

		/**
		 * @brief Return true if the vsync is currently enabled
		 * @return 
		 */
		bool HasVsync() const;

		/**
		* Enable or disable the vsync
		* @note You must call this method after creating and defining a window as the current context
		* @param p_value (True to enable vsync)
		*/

		/**
		 * @brief Enable or disable the vsync
		 * @note You must call this method after creating and defining a window as the current context
		 * @param p_value (True to enable vsync)
		 */
		void SetVsync(bool p_value);

		/**
		 * @brief Enable the inputs and events managments with created windows
		 * @note Should be called every frames
		 */
		void PollEvents() const;

		/**
		 * @brief Returns the elapsed time since the device startup
		 * @return 
		 */
		float GetElapsedTime() const;

	private:
		void CreateGlfwWindow(const WindowSettings& p_windowSettings);

		/* Callbacks binding */
		void BindKeyCallback() const;
		void BindMouseCallback() const;
		void BindResizeCallback() const;
		void BindFramebufferResizeCallback() const;
		void BindCursorMoveCallback() const;
		void BindScrollMoveCallback() const;
		void BindMoveCallback() const;
		void BindIconifyCallback() const;
		void BindFocusCallback() const;
		void BindCloseCallback() const;

		/* Event listeners */
		void OnResize(uint16_t p_width, uint16_t p_height);
		void OnMove(int16_t p_x, int16_t p_y);

		/* Internal helpers */
		void UpdateSizeLimit() const;


		void BindErrorCallback();
		void CreateCursors();
		void DestroyCursors();
	public:

		/**
		* Bind a listener to this event to receive device errors
		*/
		static Event<EWindowError, std::string> ErrorEvent;

		/* Inputs relatives */
		Event<int> KeyPressedEvent;
		Event<int> KeyReleasedEvent;
		Event<int> MouseButtonPressedEvent;
		Event<int> MouseButtonReleasedEvent;

		/* Window events */
		Event<uint16_t, uint16_t> ResizeEvent;
		Event<uint16_t, uint16_t> FramebufferResizeEvent;
		Event<int16_t, int16_t> MoveEvent;
		Event<int16_t, int16_t> ScrollMoveEvent;
		Event<int16_t, int16_t> CursorMoveEvent;
		Event<> MinimizeEvent;
		Event<> MaximizeEvent;
		Event<> GainFocusEvent;
		Event<> LostFocusEvent;
		Event<> CloseEvent;

	private:
		/* This map is used by callbacks to find a "Window" instance out of a "GLFWwindow" instnace*/
		static std::unordered_map<GLFWwindow*, Window*> __WINDOWS_MAP;
		
		GLFWwindow* m_glfwWindow;

		/* Window settings */
		std::string m_title;
		std::pair<uint16_t, uint16_t> m_size;
		std::pair<int16_t, int16_t> m_minimumSize;
		std::pair<int16_t, int16_t> m_maximumSize;
		std::pair<int16_t, int16_t> m_position;
		bool m_fullscreen;
		int32_t m_refreshRate;
		ECursorMode m_cursorMode;
		ECursorShape m_cursorShape;

		bool m_vsync = true;
		bool m_isAlive = false;

		std::unordered_map<ECursorShape, GLFWcursor*> m_cursors;

	};
}

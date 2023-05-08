
#pragma once

#include <string>
#include <unordered_map>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "EDeviceError.h"
#include "Runtime/Core/Tools/Eventing/Event.h"
#include "Runtime/Core/Window/Cursor/ECursorShape.h"
#include "Runtime/Core/Window/Settings/DeviceSettings.h"

namespace LitchiRuntime
{
	/**
	* The Device represents the windowing context. It is necessary to create a device
	* to create a window
	*/
	class Device
	{
	public:
		/**
		* Bind a listener to this event to receive device errors
		*/
		static Event<EDeviceError, std::string> ErrorEvent;

		/**
		* The constructor of the device will take care about GLFW initialization
		*/
		Device(const DeviceSettings& p_deviceSettings);

		/**
		* The destructor of the device will take care about GLFW destruction
		*/
		~Device();

		/**
		* Return the size, in pixels, of the primary monity
		*/
		std::pair<int16_t, int16_t> GetMonitorSize() const;

		/**
		* Return an instance of GLFWcursor corresponding to the given shape
		* @param p_cursorShape
		*/
		GLFWcursor* GetCursorInstance(ECursorShape p_cursorShape) const;

		/**
		* Return true if the vsync is currently enabled
		*/
		bool HasVsync() const;

		/**
		* Enable or disable the vsync
		* @note You must call this method after creating and defining a window as the current context
		* @param p_value (True to enable vsync)
		*/
		void SetVsync(bool p_value);

		/**
		* Enable the inputs and events managments with created windows
		* @note Should be called every frames
		*/
		void PollEvents() const;

		/**
		* Returns the elapsed time since the device startup
		*/
		float GetElapsedTime() const;

	private:
		void BindErrorCallback();
		void CreateCursors();
		void DestroyCursors();

	private:
		bool m_vsync = true;
		bool m_isAlive = false;
		std::unordered_map<ECursorShape, GLFWcursor*> m_cursors;
	};
}



#include <stdexcept>

#include "Device.h"

#include <glad/glad.h>

#include "EDeviceError.h"

LitchiRuntime::Event<LitchiRuntime::EDeviceError, std::string> LitchiRuntime::Device::ErrorEvent;

LitchiRuntime::Device::Device(const DeviceSettings& p_deviceSettings)
{
	BindErrorCallback();

	int initializationCode = glfwInit();

	if (initializationCode == GLFW_FALSE)
	{
		throw std::runtime_error("Failed to Init GLFW");
		glfwTerminate();
	}
	else
	{
		CreateCursors();

		if (p_deviceSettings.debugProfile)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, p_deviceSettings.contextMajorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, p_deviceSettings.contextMinorVersion);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, p_deviceSettings.samples);

		m_isAlive = true;
	}
}

LitchiRuntime::Device::~Device()
{
	if (m_isAlive)
	{
		DestroyCursors();
		glfwTerminate();
	}
}

std::pair<int16_t, int16_t> LitchiRuntime::Device::GetMonitorSize() const
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	return std::pair<int16_t, int16_t>(static_cast<int16_t>(mode->width), static_cast<int16_t>(mode->height));
}

GLFWcursor * LitchiRuntime::Device::GetCursorInstance(ECursorShape p_cursorShape) const
{
	return m_cursors.at(p_cursorShape);
}

bool LitchiRuntime::Device::HasVsync() const
{
	return m_vsync;
}

void LitchiRuntime::Device::SetVsync(bool p_value)
{
	glfwSwapInterval(p_value ? 1 : 0);
	m_vsync = p_value;
}

void LitchiRuntime::Device::PollEvents() const
{
	glfwPollEvents();
}

float LitchiRuntime::Device::GetElapsedTime() const
{
	return static_cast<float>(glfwGetTime());
}

void LitchiRuntime::Device::BindErrorCallback()
{
	auto errorCallback = [](int p_code, const char* p_description)
	{
		ErrorEvent.Invoke(static_cast<EDeviceError>(p_code), p_description);
	};

	glfwSetErrorCallback(errorCallback);
}

void LitchiRuntime::Device::CreateCursors()
{
	m_cursors[ECursorShape::ARROW] = glfwCreateStandardCursor(static_cast<int>(ECursorShape::ARROW));
	m_cursors[ECursorShape::IBEAM] = glfwCreateStandardCursor(static_cast<int>(ECursorShape::IBEAM));
	m_cursors[ECursorShape::CROSSHAIR] = glfwCreateStandardCursor(static_cast<int>(ECursorShape::CROSSHAIR));
	m_cursors[ECursorShape::HAND] = glfwCreateStandardCursor(static_cast<int>(ECursorShape::HAND));
	m_cursors[ECursorShape::HRESIZE] = glfwCreateStandardCursor(static_cast<int>(ECursorShape::HRESIZE));
	m_cursors[ECursorShape::VRESIZE] = glfwCreateStandardCursor(static_cast<int>(ECursorShape::VRESIZE));
}

void LitchiRuntime::Device::DestroyCursors()
{
	glfwDestroyCursor(m_cursors[ECursorShape::ARROW]);
	glfwDestroyCursor(m_cursors[ECursorShape::IBEAM]);
	glfwDestroyCursor(m_cursors[ECursorShape::CROSSHAIR]);
	glfwDestroyCursor(m_cursors[ECursorShape::HAND]);
	glfwDestroyCursor(m_cursors[ECursorShape::HRESIZE]);
	glfwDestroyCursor(m_cursors[ECursorShape::VRESIZE]);
}



#include "InputManager.h"

#include <iostream>

LitchiRuntime::InputManager::InputManager(Window& p_window) : m_window(p_window)
{
	m_keyPressedListener = m_window.KeyPressedEvent.AddListener(std::bind(&InputManager::OnKeyPressed, this, std::placeholders::_1));
	m_keyReleasedListener = m_window.KeyReleasedEvent.AddListener(std::bind(&InputManager::OnKeyReleased, this, std::placeholders::_1));
	m_mouseButtonPressedListener = m_window.MouseButtonPressedEvent.AddListener(std::bind(&InputManager::OnMouseButtonPressed, this, std::placeholders::_1));
	m_mouseButtonReleasedListener = m_window.MouseButtonReleasedEvent.AddListener(std::bind(&InputManager::OnMouseButtonReleased, this, std::placeholders::_1));
}

LitchiRuntime::InputManager::~InputManager()
{
	m_window.KeyPressedEvent.RemoveListener(m_keyPressedListener);
	m_window.KeyReleasedEvent.RemoveListener(m_keyReleasedListener);
	m_window.MouseButtonPressedEvent.RemoveListener(m_mouseButtonPressedListener);
	m_window.MouseButtonReleasedEvent.RemoveListener(m_mouseButtonReleasedListener);
}

LitchiRuntime::EKeyState LitchiRuntime::InputManager::GetKeyState(EKey p_key) const
{
	switch (glfwGetKey(m_window.GetGlfwWindow(), static_cast<int>(p_key)))
	{
		case GLFW_PRESS:	return EKeyState::KEY_DOWN;
		case GLFW_RELEASE:	return EKeyState::KEY_UP;
	}

	return EKeyState::KEY_UP;
}

LitchiRuntime::EMouseButtonState LitchiRuntime::InputManager::GetMouseButtonState(EMouseButton p_button) const
{
	switch (glfwGetMouseButton(m_window.GetGlfwWindow(), static_cast<int>(p_button)))
	{
		case GLFW_PRESS:	return EMouseButtonState::MOUSE_DOWN;
		case GLFW_RELEASE:	return EMouseButtonState::MOUSE_UP;
	}

	return EMouseButtonState::MOUSE_UP;
}

bool LitchiRuntime::InputManager::IsKeyPressed(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_DOWN;
}

bool LitchiRuntime::InputManager::IsKeyReleased(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_UP;
}

bool LitchiRuntime::InputManager::IsMouseButtonPressed(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_DOWN;
}

bool LitchiRuntime::InputManager::IsMouseButtonReleased(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_UP;
}

std::pair<double, double> LitchiRuntime::InputManager::GetMousePosition() const
{
	std::pair<double, double> result;
	glfwGetCursorPos(m_window.GetGlfwWindow(), &result.first, &result.second);
	return result;
}

void LitchiRuntime::InputManager::ClearEvents()
{
	m_keyEvents.clear();
	m_mouseButtonEvents.clear();
}

void LitchiRuntime::InputManager::OnKeyPressed(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_DOWN;
}

void LitchiRuntime::InputManager::OnKeyReleased(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_UP;
}

void LitchiRuntime::InputManager::OnMouseButtonPressed(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_DOWN;
}

void LitchiRuntime::InputManager::OnMouseButtonReleased(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_UP;
}

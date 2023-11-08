

#include "InputManager.h"
#include "Runtime/Core/Log/Debug.h"
#include <iostream>

using namespace LitchiRuntime;

Window* InputManager::m_window = nullptr;

ListenerID InputManager::m_keyPressedListener;
ListenerID InputManager::m_keyReleasedListener;
ListenerID InputManager::m_mouseButtonPressedListener;
ListenerID InputManager::m_mouseButtonReleasedListener;

std::unordered_map<EKey, EKeyState>					InputManager::m_keyEvents;
std::unordered_map<EMouseButton, EMouseButtonState>	InputManager::m_mouseButtonEvents;

Vector2 InputManager::m_mouse_position = Vector2::Zero;
Vector2 InputManager::m_mouse_delta = Vector2::Zero;
Vector2 InputManager::m_mouse_wheel_delta = Vector2::Zero;
Vector2 InputManager::m_editor_viewport_offset = Vector2::Zero;
bool InputManager::m_mouse_is_in_viewport = true;

void LitchiRuntime::InputManager::Initialize(Window* p_window)
{
	m_window = p_window;
	m_keyPressedListener = m_window->KeyPressedEvent.AddListener(InputManager::OnKeyPressed);
	m_keyReleasedListener = m_window->KeyReleasedEvent.AddListener(InputManager::OnKeyReleased);
	m_mouseButtonPressedListener = m_window->MouseButtonPressedEvent.AddListener(InputManager::OnMouseButtonPressed);
	m_mouseButtonReleasedListener = m_window->MouseButtonReleasedEvent.AddListener(InputManager::OnMouseButtonReleased);

}

void LitchiRuntime::InputManager::UnInit()
{

	m_window->KeyPressedEvent.RemoveListener(m_keyPressedListener);
	m_window->KeyReleasedEvent.RemoveListener(m_keyReleasedListener);
	m_window->MouseButtonPressedEvent.RemoveListener(m_mouseButtonPressedListener);
	m_window->MouseButtonReleasedEvent.RemoveListener(m_mouseButtonReleasedListener);
}

LitchiRuntime::EKeyState LitchiRuntime::InputManager::GetKeyState(EKey p_key)
{
	switch (glfwGetKey(m_window->GetGlfwWindow(), static_cast<int>(p_key)))
	{
		case GLFW_PRESS:	return EKeyState::KEY_DOWN;
		case GLFW_RELEASE:	return EKeyState::KEY_UP;
	}

	return EKeyState::KEY_UP;
}

LitchiRuntime::EMouseButtonState LitchiRuntime::InputManager::GetMouseButtonState(EMouseButton p_button)
{
	switch (glfwGetMouseButton(m_window->GetGlfwWindow(), static_cast<int>(p_button)))
	{
		case GLFW_PRESS:	return EMouseButtonState::MOUSE_DOWN;
		case GLFW_RELEASE:	return EMouseButtonState::MOUSE_UP;
	}

	return EMouseButtonState::MOUSE_UP;
}

bool LitchiRuntime::InputManager::IsKeyPressed(EKey p_key)
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_DOWN;
}

bool LitchiRuntime::InputManager::IsKeyReleased(EKey p_key)
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_UP;
}

bool LitchiRuntime::InputManager::IsMouseButtonPressed(EMouseButton p_button)
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_DOWN;
}

bool LitchiRuntime::InputManager::IsMouseButtonReleased(EMouseButton p_button)
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_UP;
}

void LitchiRuntime::InputManager::Tick()
{
	/*m_keys_previous_frame = m_keys;

	PollMouse();
	PollKeyboard();
	PollController();*/

	double x, y;
	glfwGetCursorPos(m_window->GetGlfwWindow(), &x, &y);
	Vector2 position = Vector2(static_cast<int>(x), static_cast<int>(y));
	// Get delta
	m_mouse_delta = position - m_mouse_position;

	// Get position
	m_mouse_position = position;

	DEBUG_LOG_INFO("mouse delta {}{}", position.x, position.y);

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


void LitchiRuntime::InputManager::SetMouseIsInViewport(const bool is_in_viewport)
{
	m_mouse_is_in_viewport = is_in_viewport;
}

bool LitchiRuntime::InputManager::GetMouseIsInViewport()
{
	return m_mouse_is_in_viewport;
}

const Vector2& LitchiRuntime::InputManager::GetMousePosition()
{
	return m_mouse_position;
}

void LitchiRuntime::InputManager::SetMousePosition(const Vector2& position)
{
	// todo: 
	/*if (SDL_WarpMouseGlobal(static_cast<int>(position.x), static_cast<int>(position.y)) != 0)
	{
		DEBUG_LOG_ERROR("Failed to set mouse position.");
		return;
	}*/

	m_mouse_position = position;
}

const Vector2& LitchiRuntime::InputManager::GetMouseDelta()
{
	return m_mouse_delta;
}

const Vector2& LitchiRuntime::InputManager::GetMouseWheelDelta()
{
	return m_mouse_wheel_delta;
}

void LitchiRuntime::InputManager::SetEditorViewportOffset(const Vector2& offset)
{
	m_editor_viewport_offset = offset;
}

bool LitchiRuntime::InputManager::GetMouseCursorVisible()
{
	// todo:
	// glfwGetCursorPos()
	// return SDL_ShowCursor(SDL_QUERY) == 1;
	return true;
}

void LitchiRuntime::InputManager::SetMouseCursorVisible(const bool visible)
{
	// todo:
   /* if (visible == GetMouseCursorVisible())
		return;

	if (SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE) < 0)
	{
		SP_LOG_ERROR("Failed to change cursor visibility");
	}*/
}

const Vector2 LitchiRuntime::InputManager::GetMousePositionRelativeToWindow()
{
	int window_x, window_y;

	// GLFWwindow* window = static_cast<GLFWwindow*>(ApplicationBase::Instance()->window->GetGlfwWindow());
	
	// glfwGetWindowPos(window, &window_x, &window_y);
	return Vector2(static_cast<float>(m_mouse_position.x - window_x), static_cast<float>(m_mouse_position.y - window_y));
}

const Vector2 LitchiRuntime::InputManager::GetMousePositionRelativeToEditorViewport()
{
	return GetMousePositionRelativeToWindow() - m_editor_viewport_offset;
}


#include "input.h"
#include <iostream>
#include "KeyCode.h"
#include "Runtime/Core/Log/debug.h"
#include "GLFW/glfw3.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Core/Window/Window.h"

namespace LitchiRuntime
{
	// Mouse
	static Vector2 m_mouse_position = Vector2::Zero;
	static Vector2 m_mouse_delta = Vector2::Zero;
	static Vector2 m_mouse_wheel_delta = Vector2::Zero;
	static Vector2 m_editor_viewport_offset = Vector2::Zero;
	static bool m_mouse_is_in_viewport = true;

	std::unordered_map<unsigned short, unsigned short> Input::key_event_map_;
	glm::vec2 Input::mouse_position_ = { 0,0 };
	short Input::mouse_scroll_ = 0;

	bool Input::GetKey(unsigned short key_code) {
		return key_event_map_.count(key_code) > 0;
	}

	bool Input::GetKeyDown(unsigned short key_code) {
		if (key_event_map_.count(key_code) == 0) {
			return false;
		}
		return key_event_map_[key_code] != KEY_ACTION_UP;
	}

	bool Input::GetKeyUp(unsigned short key_code) {
		if (key_event_map_.count(key_code) == 0) {
			return false;
		}
		return key_event_map_[key_code] == KEY_ACTION_UP;
	}

	bool Input::GetMouseButton(unsigned short mouse_button_index) {
		return GetKey(mouse_button_index);
	}

	bool Input::GetMouseButtonDown(unsigned short mouse_button_index) {
		return GetKeyDown(mouse_button_index);
	}

	bool Input::GetMouseButtonUp(unsigned short mouse_button_index) {
		return GetKeyUp(mouse_button_index);
	}

	void Input::RecordKey(unsigned short key_code, unsigned short key_action) {
		key_event_map_[key_code] = key_action;
	}

	void Input::Update() {
		for (auto iterator = key_event_map_.begin(); iterator != key_event_map_.end();) {
			if (iterator->second == KEY_ACTION_UP) {
				iterator = key_event_map_.erase(iterator);    //删除元素，返回值指向已删除元素的下一个位置
			}
			else {
				++iterator;    //指向下一个位置
			}
		}

		mouse_scroll_ = 0;
	}


	void Input::SetMouseIsInViewport(const bool is_in_viewport)
	{
		m_mouse_is_in_viewport = is_in_viewport;
	}

	bool Input::GetMouseIsInViewport()
	{
		return m_mouse_is_in_viewport;
	}

	const Vector2& Input::GetMousePosition()
	{
		return m_mouse_position;
	}

	void Input::SetMousePosition(const Vector2& position)
	{
		// todo: 
		/*if (SDL_WarpMouseGlobal(static_cast<int>(position.x), static_cast<int>(position.y)) != 0)
		{
			DEBUG_LOG_ERROR("Failed to set mouse position.");
			return;
		}*/

		m_mouse_position = position;
	}

	const Vector2& Input::GetMouseDelta()
	{
		return m_mouse_delta;
	}

	const Vector2& Input::GetMouseWheelDelta()
	{
		return m_mouse_wheel_delta;
	}

	void Input::SetEditorViewportOffset(const Vector2& offset)
	{
		m_editor_viewport_offset = offset;
	}

	bool Input::GetMouseCursorVisible()
	{
		// todo:
		// glfwGetCursorPos()
		// return SDL_ShowCursor(SDL_QUERY) == 1;
		return true;
	}

	void Input::SetMouseCursorVisible(const bool visible)
	{
		// todo:
	   /* if (visible == GetMouseCursorVisible())
			return;

		if (SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE) < 0)
		{
			SP_LOG_ERROR("Failed to change cursor visibility");
		}*/
	}

	const Vector2 Input::GetMousePositionRelativeToWindow()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(ApplicationBase::Instance()->window->GetGlfwWindow());
		int window_x, window_y;

		glfwGetWindowPos(window, &window_x, &window_y);
		return Vector2(static_cast<float>(m_mouse_position.x - window_x), static_cast<float>(m_mouse_position.y - window_y));
	}

	const Vector2 Input::GetMousePositionRelativeToEditorViewport()
	{
		return GetMousePositionRelativeToWindow() - m_editor_viewport_offset;
	}

}

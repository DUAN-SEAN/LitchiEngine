
#pragma once

#include <unordered_map>

#include "EKey.h"
#include "EKeyState.h"
#include "EMouseButton.h"
#include "EMouseButtonState.h"
#include "Runtime/Core/Math/Vector2.h"
#include "Runtime/Core/Window/Window.h"

namespace LitchiRuntime
{
	/// <summary>
	/// Handles inputs (Mouse and keyboard)
	/// </summary>
	class InputManager
	{
	public:
		/**
		 * @brief Initialize
		 * @param p_window 
		*/
		static void Initialize(Window* p_window);

		/**
		 * @brief UnInitialize
		*/
		static void UnInitialize();

		/**
		 * @brief Tick
		*/
		static void Tick();

		/**
		 * @brief Clear any event occur
		 * Should be called at the end of every game tick
		*/
		static void ClearEvents();

		/**
		 * @brief Return the current state of the given key
		 * @param p_key 
		 * @return 
		*/
		static EKeyState GetKeyState(EKey p_key);

		/**
		* Return the current state of the given mouse button
		* @param p_button
		*/
		static EMouseButtonState GetMouseButtonState(EMouseButton p_button);

		/**
		* Return true if the given key has been pressed during the frame
		* @param p_key
		*/
		static bool IsKeyPressed(EKey p_key);

		/**
		* Return true if the given key has been released during the frame
		* @param p_key
		*/
		static bool IsKeyReleased(EKey p_key);

		/**
		* Return true if the given mouse button has been pressed during the frame
		* @param p_button
		*/
		static bool IsMouseButtonPressed(EMouseButton p_button);

		/**
		* Return true if the given mouse button has been released during the frame
		* @param p_button
		*/
		static bool IsMouseButtonReleased(EMouseButton p_button);

		static void SetMouseCursorVisible(const bool visible);
		static bool GetMouseCursorVisible();
		static void SetMouseIsInViewport(const bool is_in_viewport);
		static bool GetMouseIsInViewport();
		static const Vector2& GetMousePosition();
		static void SetMousePosition(const Vector2& position);
		static const Vector2& GetMouseDelta();
		static const Vector2& GetMouseWheelDelta();
		static void SetEditorViewportOffset(const Vector2& offset);
		static const Vector2 GetMousePositionRelativeToWindow();
		static const Vector2 GetMousePositionRelativeToEditorViewport();

	private:
		static void OnKeyPressed(int p_key);
		static void OnKeyReleased(int p_key);
		static void OnMouseButtonPressed(int p_button);
		static void OnMouseButtonReleased(int p_button);
		static void OnScrollMove(int16_t x, int16_t y);

	private:
		static Window* m_window;

		static ListenerID m_keyPressedListener;
		static ListenerID m_keyReleasedListener;
		static ListenerID m_mouseButtonPressedListener;
		static ListenerID m_mouseButtonReleasedListener;
		static ListenerID m_scrollMoveListener;

		static std::unordered_map<EKey, EKeyState>					m_keyEvents;
		static std::unordered_map<EMouseButton, EMouseButtonState>	m_mouseButtonEvents;

		//// Mouse
		static Vector2 m_mouse_position;
		static Vector2 m_mouse_delta;
		static Vector2 m_mouse_wheel_delta;
		static Vector2 m_editor_viewport_offset;
		static bool m_mouse_is_in_viewport;
	};
}
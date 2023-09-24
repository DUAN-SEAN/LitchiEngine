
//= INCLUDES ==============
#include "Runtime/Core/pch.h"
#include "Input.h"
#include "../Window.h"
SP_WARNINGS_OFF
#include <SDL.h>
SP_WARNINGS_ON
//=========================

//= NAMESPACES ===============
using namespace std;
using namespace Spartan::Math;
//============================

namespace Spartan
{
    // Mouse
    static Math::Vector2 m_mouse_position         = Math::Vector2::Zero;
    static Math::Vector2 m_mouse_delta            = Math::Vector2::Zero;
    static Math::Vector2 m_mouse_wheel_delta      = Math::Vector2::Zero;
    static Math::Vector2 m_editor_viewport_offset = Math::Vector2::Zero;
    static bool m_mouse_is_in_viewport            = true;

    void Input::PostTick()
    {
        m_mouse_wheel_delta = Vector2::Zero;
    }

    void Input::PollMouse()
    {
        // Get state
        int x, y;
        Uint32 keys_states = SDL_GetGlobalMouseState(&x, &y);
        Vector2 position   = Vector2(static_cast<float>(x), static_cast<float>(y));

        // Get delta
        m_mouse_delta = position - m_mouse_position;

        // Get position
        m_mouse_position = position;

        // Get keys
        GetKeys()[m_start_index_mouse]     = (keys_states & SDL_BUTTON(SDL_BUTTON_LEFT))   != 0; // Left button pressed
        GetKeys()[m_start_index_mouse + 1] = (keys_states & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0; // Middle button pressed
        GetKeys()[m_start_index_mouse + 2] = (keys_states & SDL_BUTTON(SDL_BUTTON_RIGHT))  != 0; // Right button pressed
    }

    void Input::OnEventMouse(void* event_mouse)
    {
        // Validate event
        SP_ASSERT(event_mouse != nullptr);
        SDL_Event* sdl_event = static_cast<SDL_Event*>(event_mouse);
        Uint32 event_type = sdl_event->type;

        // Wheel
        if (event_type == SDL_MOUSEWHEEL)
        {
            if (sdl_event->wheel.x > 0) m_mouse_wheel_delta.x += 1;
            if (sdl_event->wheel.x < 0) m_mouse_wheel_delta.x -= 1;
            if (sdl_event->wheel.y > 0) m_mouse_wheel_delta.y += 1;
            if (sdl_event->wheel.y < 0) m_mouse_wheel_delta.y -= 1;
        }
    }

    bool Input::GetMouseCursorVisible()
    {
        return SDL_ShowCursor(SDL_QUERY) == 1;
    }

    void Input::SetMouseCursorVisible(const bool visible)
    {
        if (visible == GetMouseCursorVisible())
            return;

        if (SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE) < 0)
        {
            DEBUG_LOG_ERROR("Failed to change cursor visibility");
        }
    }

    const Vector2 Input::GetMousePositionRelativeToWindow()
    {
        SDL_Window* window = static_cast<SDL_Window*>(Window::GetHandleSDL());
        int window_x, window_y;
        SDL_GetWindowPosition(window, &window_x, &window_y);
        return Vector2(static_cast<float>(m_mouse_position.x - window_x), static_cast<float>(m_mouse_position.y - window_y));
    }

    const Vector2 Input::GetMousePositionRelativeToEditorViewport()
    {
        return GetMousePositionRelativeToWindow() - m_editor_viewport_offset;
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

    void Input::SetMousePosition(const Math::Vector2& position)
    {
        if (SDL_WarpMouseGlobal(static_cast<int>(position.x), static_cast<int>(position.y)) != 0)
        {
            DEBUG_LOG_ERROR("Failed to set mouse position.");
            return;
        }

        m_mouse_position = position;
    }

    const Spartan::Math::Vector2& Input::GetMouseDelta()
    {
        return m_mouse_delta;
    }

    const Spartan::Math::Vector2& Input::GetMouseWheelDelta()
    {
        return m_mouse_wheel_delta;
    }

    void Input::SetEditorViewportOffset(const Math::Vector2& offset)
    {
        m_editor_viewport_offset = offset;
    }
}

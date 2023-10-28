
#pragma once

#include <unordered_map>

#include <Runtime/Core/Math/Vector2.h>

namespace LitchiRuntime
{

    class Input {
    public:
        /// 记录按键事件，键盘按下记录数+1，键盘弹起记录数-1，当记录数为0，说明此时没有按键。
        /// \param key_code
        /// \param action 0松手 1按下 2持续按下
        static void RecordKey(unsigned short key_code, unsigned short key_action);

        /// 判断当前帧 键盘按键是否按下
        /// \param key_code
        /// \return
        static bool GetKey(unsigned short key_code);

        /// 判断当前帧 键盘按键是否处于按下状态
        /// \param key_code
        /// \return
        static bool GetKeyDown(unsigned short key_code);

        /// 判断当前帧 键盘按键是否按下并松开
        /// \param key_code
        /// \return
        static bool GetKeyUp(unsigned short key_code);

        /// 刷帧
        static void Update();

        /// 判断是否按了鼠标某个按钮
        /// \param mouse_button_index   0 表示主按钮（通常为左按钮），1 表示副按钮，2 表示中间按钮。
        /// \return
        static bool GetMouseButton(unsigned short mouse_button_index);

        /// 指定鼠标按键是否处于按下的状态
        /// \param mouse_button_index   0 表示主按钮（通常为左按钮），1 表示副按钮，2 表示中间按钮。
        /// \return
        static bool GetMouseButtonDown(unsigned short mouse_button_index);

        /// 鼠标按钮是否松开
        /// \param mouse_button_index   0 表示主按钮（通常为左按钮），1 表示副按钮，2 表示中间按钮。
        /// \return
        static bool GetMouseButtonUp(unsigned short mouse_button_index);

        static Vector2 mousePosition() { return mouse_position_; }
        /// 设置鼠标位置
        /// \param x
        /// \param y
        static void set_mousePosition(float x, float y) { mouse_position_.x = x; mouse_position_.y = y; }

        static short mouse_scroll() { return mouse_scroll_; }
        /// 记录鼠标滚轮事件
        /// \param mouse_scroll
        static void RecordScroll(short mouse_scroll) { mouse_scroll_ += mouse_scroll; }

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
        static std::unordered_map<unsigned short, unsigned short> key_event_map_;//按键状态 奇数按下 偶数弹起 0表示没有记录
        static Vector2 mouse_position_;//鼠标位置
        static short mouse_scroll_;//鼠标滚轮值
    };
}

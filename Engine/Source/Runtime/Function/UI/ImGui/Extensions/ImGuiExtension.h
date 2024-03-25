
#pragma once

//= INCLUDES =============================
#include <string>
#include <variant>
#include <chrono>

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Core/Window/Window.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
#include "Runtime/Function/Renderer/RHI//RHI_Texture2D.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
#include "Runtime/Function/Renderer/Rendering/Mesh.h"
#include "Runtime/Function/Renderer/Resource/ResourceCache.h"
#include "Runtime/Function/UI/Base/IconLoader.h"
#include "Runtime/Function/UI/ImGui/imgui_internal.h"

//========================================
//
//class EditorHelper
//{
//public:
//    static void Initialize(Editor* editor_)
//    {
//        editor = editor_;
//    }
//
//    static void LoadMesh(const std::string& file_path, const uint32_t mesh_flags)
//    {
//        // load the model asynchronously
//        LitchiRuntime::ThreadPool::AddTask([file_path, mesh_flags]()
//        {
//            LitchiRuntimeCache::Load<LitchiRuntime::Mesh>(file_path, mesh_flags);
//        });
//    }
//
//    static void LoadWorld(const std::string& file_path)
//    {
//        // loading a world resets everything so it's important to ensure that no tasks are running
//        LitchiRuntime::ThreadPool::Flush(true);
//
//        // load the scene asynchronously
//        LitchiRuntime::ThreadPool::AddTask([file_path]()
//        {
//            LitchiRuntime::World::LoadFromFile(file_path);
//        });
//    }
//
//    static void SaveWorld(const std::string& file_path)
//    {
//        // save the scene asynchronously
//        LitchiRuntime::ThreadPool::AddTask([file_path]()
//        {
//            LitchiRuntime::World::SaveToFile(file_path);
//        });
//    }
//
//    static Editor* editor;
//};

namespace ImGuiSp
{
    enum class DragPayloadType
    {
        Texture,
        Entity,
        Model,
        Audio,
        Material,
        Undefined
    };

    enum class ButtonPress
    {
        Yes,
        No,
        Undefined
    };

    static const ImVec4 default_tint(1, 1, 1, 1);

    static float GetWindowContentRegionWidth()
    {
        return ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    }

    // Collapsing header
    static bool collapsing_header(const char* label, ImGuiTreeNodeFlags flags = 0)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        bool result = ImGui::CollapsingHeader(label, flags);
        ImGui::PopStyleVar();
        return result;
    }

    // Button
    static bool button(const char* label, const ImVec2& size = ImVec2(0, 0))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushID(static_cast<int>(ImGui::GetCursorPosX() + ImGui::GetCursorPosY()));
        bool result = ImGui::Button(label, size);
        ImGui::PopID();
        ImGui::PopStyleVar();
        return result;
    }

    static bool button_centered_on_line(const char* label, float alignment = 0.5f)
    {
        ImGuiStyle& style = ImGui::GetStyle();

        float size  = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - size) * alignment;
        if (off > 0.0f)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }

        return ImGui::Button(label);
    }

    static bool image_button(uint64_t id, LitchiRuntime::RHI_Texture* texture, const IconType icon, const LitchiRuntime::Vector2& size, bool border)
    {
        if (!border)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        }

        // Get texture from icon enum (if provided)
        if (!texture && icon != IconType::Undefined)
        {
            texture = IconLoader::GetTextureByType(icon);
        }

        // Compute ID
        id += static_cast<uint64_t>(icon);
        id += texture ? reinterpret_cast<uint64_t>(&texture) : 0;

        bool result = ImGui::ImageButton
        (
            std::to_string(id).c_str(),        // str_id
            static_cast<ImTextureID>(texture), // user_texture_id
            size,                              // size
            ImVec2(0, 0),                      // uv0
            ImVec2(1, 1),                      // uv1
            ImColor(0, 0, 0, 0),               // bg_col
            default_tint                       // tint_col
        );

        if (!border)
        {
            ImGui::PopStyleVar();
        }

        return result;
    }

    static void image(const Icon& icon, const float size)
    {
        ImGui::Image(
            static_cast<ImTextureID>(icon.GetTexture()),
            ImVec2(size, size),
            ImVec2(0, 0),
            ImVec2(1, 1),
            default_tint,       // tint
            ImColor(0, 0, 0, 0) // border
        );
    }

    static void image(LitchiRuntime::RHI_Texture* texture, const LitchiRuntime::Vector2& size, bool border = false)
    {
        if (!border)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        }

        ImGui::Image(
            static_cast<ImTextureID>(texture),
            size,
            ImVec2(0, 0),
            ImVec2(1, 1),
            default_tint,       // tint
            ImColor(0, 0, 0, 0) // border
        );

        if (!border)
        {
            ImGui::PopStyleVar();
        }
    }

    static void image(LitchiRuntime::RHI_Texture* texture, const ImVec2& size, const ImVec4& tint = default_tint, const ImColor& border = ImColor(0, 0, 0, 0))
    {
        ImGui::Image(
            static_cast<ImTextureID>(texture),
            size,
            ImVec2(0, 0),
            ImVec2(1, 1),
            tint,
            border
        );
    }

    static void image(const IconType icon, const float size)
    {
        ImGui::Image(
            static_cast<void*>(IconLoader::GetTextureByType(icon)),
            ImVec2(size, size),
            ImVec2(0, 0),
            ImVec2(1, 1),
            default_tint,       // tint
            ImColor(0, 0, 0, 0) // border
        );
    }

    struct DragDropPayload
    {
        using DataVariant = std::variant<const char*, uint64_t>;
        DragDropPayload(const DragPayloadType type = DragPayloadType::Undefined, const DataVariant data = nullptr)
        {
            this->type = type;
            this->data = data;
        }
        DragPayloadType type;
        DataVariant data;
    };

    static void create_drag_drop_paylod(const DragDropPayload& payload)
    {
        ImGui::SetDragDropPayload(reinterpret_cast<const char*>(&payload.type), reinterpret_cast<const void*>(&payload), sizeof(payload), ImGuiCond_Once);
    }

    static DragDropPayload* receive_drag_drop_payload(DragPayloadType type)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto payload_imgui = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&type)))
            {
                return static_cast<DragDropPayload*>(payload_imgui->Data);
            }
            ImGui::EndDragDropTarget();
        }

        return nullptr;
    }

    // Image slot
    static void image_slot(const std::shared_ptr<LitchiRuntime::RHI_Texture>& texture_in, const std::function<void(LitchiRuntime::RHI_Texture*)>& setter)
    {
        const ImVec2 slot_size  = ImVec2(80 * LitchiRuntime::ApplicationBase::Instance()->window->GetDpiScale());
        const float button_size = 15.0f * LitchiRuntime::ApplicationBase::Instance()->window->GetDpiScale();

        // Image
        ImGui::BeginGroup();
        {
            LitchiRuntime::RHI_Texture* texture = texture_in.get();
            const ImVec2 pos_image        = ImGui::GetCursorPos();
            const ImVec2 pos_button       = ImVec2(ImGui::GetCursorPosX() + slot_size.x - button_size * 2.0f + 6.0f, ImGui::GetCursorPosY() + 1.0f);

            uint32_t id = static_cast<uint32_t>(pos_button.x + pos_button.y);

            // Remove button
            if (texture != nullptr)
            {
                ImGui::SetCursorPos(pos_button);
                if (image_button(id, nullptr, IconType::Component_Material_RemoveTexture, button_size, true))
                {
                    texture = nullptr;
                    setter(nullptr);
                }
            }

            // Image
            ImVec4 colro_tint   = (texture != nullptr) ? ImVec4(1, 1, 1, 1) : ImVec4(0, 0, 0, 0);
            ImVec4 color_border = ImVec4(1, 1, 1, 0.5f);
            ImGui::SetCursorPos(pos_image);
            image(texture, slot_size, colro_tint, color_border);

            // Remove button - Does nothing, drawn again just to be visible
            if (texture != nullptr)
            {
                ImGui::SetCursorPos(pos_button);
                image_button(id, nullptr, IconType::Component_Material_RemoveTexture, button_size, true);
            }
        }
        ImGui::EndGroup();

        // Drop target
        if (auto payload = receive_drag_drop_payload(DragPayloadType::Texture))
        {
            try
            {
                if (const auto tex = LitchiRuntime::ApplicationBase::Instance()->textureManager->LoadResource(std::get<const char*>(payload->data)))
                {
                    setter(tex);
                }
            }
            catch (const std::bad_variant_access& e) { DEBUG_LOG_ERROR("%s", e.what()); }
        }
    }

    static void tooltip(const char* text)
    {
        LC_ASSERT_MSG(text != nullptr, "Text is null");

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text(text);
            ImGui::EndTooltip();
        }
    }

    // A drag float which will wrap the mouse cursor around the edges of the screen
    static void draw_float_wrap(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const ImGuiSliderFlags flags = 0)
    {
        static const uint32_t screen_edge_padding = 10;
        static bool needs_to_wrap                 = false;
        ImGuiIO& imgui_io                         = ImGui::GetIO();

        // Wrap
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 position_cursor       = imgui_io.MousePos;
            float position_left          = static_cast<float>(screen_edge_padding);
            float position_right         = static_cast<float>(LitchiRuntime::ApplicationBase::Instance()->window->GetWidth() - screen_edge_padding);
            bool is_on_right_screen_edge = position_cursor.x >= position_right;
            bool is_on_left_screen_edge  = position_cursor.x <= position_left;

            if (is_on_right_screen_edge)
            {
                position_cursor.x = position_left + 1;
                needs_to_wrap     = true;
            }
            else if (is_on_left_screen_edge)
            {
                position_cursor.x = position_right - 1;
                needs_to_wrap     = true;
            }

            if (needs_to_wrap)
            {
                // set mouse position
                imgui_io.MousePos        = position_cursor;
                imgui_io.WantSetMousePos = true;

                // prevent delta from being huge by invalidating the previous position
                imgui_io.MousePosPrev = ImVec2(-FLT_MAX, -FLT_MAX);

                needs_to_wrap = false;
            }
        }

        ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, flags);
    }

    static bool combo_box(const char* label, const std::vector<std::string>& options, uint32_t* selection_index)
    {
        // Clamp the selection index in case it's larger than the actual option count.
        const uint32_t option_count = static_cast<uint32_t>(options.size());
        if (*selection_index >= option_count)
        {
            *selection_index = option_count - 1;
        }

        bool selection_made          = false;
        std::string selection_string = options[*selection_index];

        if (ImGui::BeginCombo(label, selection_string.c_str()))
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(options.size()); i++)
            {
                const bool is_selected = *selection_index == i;

                if (ImGui::Selectable(options[i].c_str(), is_selected))
                {
                    *selection_index    = i;
                    selection_made      = true;
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        return selection_made;
    }

    static void vector3(const char* label, LitchiRuntime::Vector3& vector)
    {
        const float label_indetation = 15.0f *
            LitchiRuntime::ApplicationBase::Instance()->window->GetDpiScale();

        const auto show_float = [](LitchiRuntime::Vector3 axis, float* value)
        {
            const float label_float_spacing = 15.0f *
                LitchiRuntime::ApplicationBase::Instance()->window->GetDpiScale();
            const float step                = 0.01f;

            // Label
            ImGui::TextUnformatted(axis.x == 1.0f ? "x" : axis.y == 1.0f ? "y" : "z");
            ImGui::SameLine(label_float_spacing);
            LitchiRuntime::Vector2 pos_post_label = ImGui::GetCursorScreenPos();

            // Float
            ImGui::PushItemWidth(128.0f);
            ImGui::PushID(static_cast<int>(ImGui::GetCursorPosX() + ImGui::GetCursorPosY()));
            ImGuiSp::draw_float_wrap("##no_label", value, step, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), "%.4f");
            ImGui::PopID();
            ImGui::PopItemWidth();

            // Axis color
            static const ImU32 color_x                 = IM_COL32(168, 46, 2, 255);
            static const ImU32 color_y                 = IM_COL32(112, 162, 22, 255);
            static const ImU32 color_z                 = IM_COL32(51, 122, 210, 255);
            static const LitchiRuntime::Vector2 size   = LitchiRuntime::Vector2(4.0f, 19.0f);
            static const LitchiRuntime::Vector2 offset = LitchiRuntime::Vector2(5.0f, 4.0);
            pos_post_label += offset;
            ImRect axis_color_rect = ImRect(pos_post_label.x, pos_post_label.y, pos_post_label.x + size.x, pos_post_label.y + size.y);
            ImGui::GetWindowDrawList()->AddRectFilled(axis_color_rect.Min, axis_color_rect.Max, axis.x == 1.0f ? color_x : axis.y == 1.0f ? color_y : color_z);
        };

        ImGui::BeginGroup();
        ImGui::Indent(label_indetation);
        ImGui::TextUnformatted(label);
        ImGui::Unindent(label_indetation);
        show_float(LitchiRuntime::Vector3(1.0f, 0.0f, 0.0f), &vector.x);
        show_float(LitchiRuntime::Vector3(0.0f, 1.0f, 0.0f), &vector.y);
        show_float(LitchiRuntime::Vector3(0.0f, 0.0f, 1.0f), &vector.z);
        ImGui::EndGroup();
    };

    inline ButtonPress window_yes_no(const char* title, const char* text)
    {
        // Set position
        ImVec2 position     = ImVec2(
            LitchiRuntime::ApplicationBase::Instance()->window->GetWidth() * 0.5f, LitchiRuntime::ApplicationBase::Instance()->window->GetHeight() * 0.5f);
        ImVec2 pivot_center = ImVec2(0.5f, 0.5f);
        ImGui::SetNextWindowPos(position, ImGuiCond_Always, pivot_center);

        // Window
        ButtonPress button_press = ButtonPress::Undefined;
        if (ImGui::Begin(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Text(text);

            if (ImGuiSp::button_centered_on_line("Yes", 0.4f))
            {
                button_press = ButtonPress::Yes;
            }

            ImGui::SameLine();

            if (ImGui::Button("No"))
            {
                button_press = ButtonPress::No;
            }
        }
        ImGui::End();

        return button_press;
    }
}

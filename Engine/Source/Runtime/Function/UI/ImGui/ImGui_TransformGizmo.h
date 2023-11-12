
#pragma once

//= INCLUDES ===========================
#include "ImGuizmo/ImGuizmo.h"
#include "imgui.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"

#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
//======================================

using namespace LitchiRuntime;
namespace ImGui::TransformGizmo
{
    static void apply_style()
    {
        ImGuizmo::Style& style           = ImGuizmo::GetStyle();
        style.TranslationLineThickness   = 6.0f;
        style.TranslationLineArrowSize   = 10.0f;
        style.RotationLineThickness      = 4.0f;
        style.RotationOuterLineThickness = 5.0f;
        style.ScaleLineThickness         = 6.0f;
        style.ScaleLineCircleSize        = 6.0f;
        style.HatchedAxisLineThickness   = 6.0f;
        style.CenterCircleSize           = 6.0f;
    }

    //static void tick()
    //{
    //    auto camera = LitchiRuntime::Renderer::GetCamera();
    //    if (!camera)
    //        return;

    //    // Get selected entity
    //    auto entity = camera->GetSelectedEntity();

    //    // Enable/disable gizmo
    //    ImGuizmo::Enable(entity != nullptr);
    //    if (!entity)
    //        return;

    //    // Switch between position, rotation and scale operations, with W, E and R respectively
    //    static ImGuizmo::OPERATION transform_operation = ImGuizmo::TRANSLATE;
    //    // if (!camera->IsControledInFirstPerson())
    //    {
    //        if (LitchiRuntime::InputManager::GetKeyState(LitchiRuntime::EKey::KEY_W) == LitchiRuntime::EKeyState::KEY_DOWN)
    //        {
    //            transform_operation = ImGuizmo::TRANSLATE;
    //        }
    //        else if (LitchiRuntime::InputManager::GetKeyState(LitchiRuntime::EKey::KEY_E) == LitchiRuntime::EKeyState::KEY_DOWN)
    //        {
    //            transform_operation = ImGuizmo::ROTATE;
    //        }
    //        else if (LitchiRuntime::InputManager::GetKeyState(LitchiRuntime::EKey::KEY_R) == LitchiRuntime::EKeyState::KEY_DOWN)
    //        {
    //            transform_operation = ImGuizmo::SCALE;
    //        }
    //    }

    //    ImGuizmo::MODE transform_space = ImGuizmo::WORLD;

    //    // Get some data
    //    const LitchiRuntime::Matrix& matrix_projection = camera->GetProjectionMatrix().Transposed();
    //    const LitchiRuntime::Matrix& matrix_view       = camera->GetViewMatrix().Transposed();
    //    auto transform  = entity->GetComponent<LitchiRuntime::Transform>();

    //    // Begin
    //    const bool is_orthographic = false;
    //    ImGuizmo::SetOrthographic(is_orthographic);
    //    ImGuizmo::BeginFrame();

    //    // Map transform to ImGuizmo
    //    LitchiRuntime::Vector3 position    = transform->GetPosition();
    //    LitchiRuntime::Vector3 scale       = transform->GetScale();
    //    LitchiRuntime::Quaternion rotation = transform->GetRotation();

    //    LitchiRuntime::Matrix transform_matrix = LitchiRuntime::Matrix::GenerateRowFirst(position, rotation, scale);

    //    // Set viewport rectangle
    //    ImGuizmo::SetDrawlist();
    //    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

    //    ImGuizmo::Manipulate(&matrix_view.m00, &matrix_projection.m00, transform_operation, transform_space, &transform_matrix.m00, nullptr, nullptr);

    //    // Map ImGuizmo to transform
    //    if (ImGuizmo::IsUsing())
    //    {
    //        transform_matrix.Transposed().Decompose(scale, rotation, position);

    //        transform->SetPosition(position);
    //        transform->SetRotation(rotation);
    //        transform->SetScale(scale);
    //    }
    //}

    static bool allow_picking()
    {
        return !ImGuizmo::IsOver() && !ImGuizmo::IsUsing();
    }
}

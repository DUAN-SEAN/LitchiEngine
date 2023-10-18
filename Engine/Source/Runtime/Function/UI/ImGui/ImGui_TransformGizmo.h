//
//#pragma once
//
////= INCLUDES ===========================
//#include "ImGuizmo/ImGuizmo.h"
//#include "imgui.h"
//#include "World/Components/Transform.h"
//#include "World/Entity.h"
//#include "Rendering/Renderer.h"
//#include "Input/Input.h"
////======================================
//
//namespace ImGui::TransformGizmo
//{
//    static void apply_style()
//    {
//        ImGuizmo::Style& style           = ImGuizmo::GetStyle();
//        style.TranslationLineThickness   = 6.0f;
//        style.TranslationLineArrowSize   = 10.0f;
//        style.RotationLineThickness      = 4.0f;
//        style.RotationOuterLineThickness = 5.0f;
//        style.ScaleLineThickness         = 6.0f;
//        style.ScaleLineCircleSize        = 6.0f;
//        style.HatchedAxisLineThickness   = 6.0f;
//        style.CenterCircleSize           = 6.0f;
//    }
//
//    static void tick()
//    {
//        std::shared_ptr<LitchiRuntime::Camera> camera = LitchiRuntime::Renderer::GetCamera();
//        if (!camera)
//            return;
//
//        // Get selected entity
//        std::shared_ptr<LitchiRuntime::Entity> entity = camera->GetSelectedEntity();
//
//        // Enable/disable gizmo
//        ImGuizmo::Enable(entity != nullptr);
//        if (!entity)
//            return;
//
//        // Switch between position, rotation and scale operations, with W, E and R respectively
//        static ImGuizmo::OPERATION transform_operation = ImGuizmo::TRANSLATE;
//        if (!camera->IsControledInFirstPerson())
//        {
//            if (LitchiRuntime::Input::GetKeyDown(LitchiRuntime::KeyCode::W))
//            {
//                transform_operation = ImGuizmo::TRANSLATE;
//            }
//            else if (LitchiRuntime::Input::GetKeyDown(LitchiRuntime::KeyCode::E))
//            {
//                transform_operation = ImGuizmo::ROTATE;
//            }
//            else if (LitchiRuntime::Input::GetKeyDown(LitchiRuntime::KeyCode::R))
//            {
//                transform_operation = ImGuizmo::SCALE;
//            }
//        }
//
//        ImGuizmo::MODE transform_space = ImGuizmo::WORLD;
//
//        // Get some data
//        const LitchiRuntime::Math::Matrix& matrix_projection = camera->GetProjectionMatrix().Transposed();
//        const LitchiRuntime::Math::Matrix& matrix_view       = camera->GetViewMatrix().Transposed();
//        std::shared_ptr<LitchiRuntime::Transform> transform  = entity->GetComponent<LitchiRuntime::Transform>();
//
//        // Begin
//        const bool is_orthographic = false;
//        ImGuizmo::SetOrthographic(is_orthographic);
//        ImGuizmo::BeginFrame();
//
//        // Map transform to ImGuizmo
//        LitchiRuntime::Math::Vector3 position    = transform->GetPosition();
//        LitchiRuntime::Math::Vector3 scale       = transform->GetScale();
//        LitchiRuntime::Math::Quaternion rotation = transform->GetRotation();
//
//        LitchiRuntime::Math::Matrix transform_matrix = LitchiRuntime::Math::Matrix::GenerateRowFirst(position, rotation, scale);
//
//        // Set viewport rectangle
//        ImGuizmo::SetDrawlist();
//        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
//
//        ImGuizmo::Manipulate(&matrix_view.m00, &matrix_projection.m00, transform_operation, transform_space, &transform_matrix.m00, nullptr, nullptr);
//
//        // Map ImGuizmo to transform
//        if (ImGuizmo::IsUsing())
//        {
//            transform_matrix.Transposed().Decompose(scale, rotation, position);
//
//            transform->SetPosition(position);
//            transform->SetRotation(rotation);
//            transform->SetScale(scale);
//        }
//    }
//
//    static bool allow_picking()
//    {
//        return !ImGuizmo::IsOver() && !ImGuizmo::IsUsing();
//    }
//}

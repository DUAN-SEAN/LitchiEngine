
#pragma once

#include <iostream>
#include <vector>
#include <functional>

#include "Runtime/Core/Math/Frustum.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Ray.h"
#include "Runtime/Core/Math/Rectangle.h"
#include "Runtime/Core/Math/Vector2.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Renderer/Rendering/Color.h"
#include "Runtime/Function/Renderer/RHI/RHI_Viewport.h"

namespace LitchiRuntime
{
	class MeshFilter;

    class Camera : public Component {
    public:
        Camera();
        ~Camera();
        
    public:
        void Awake() override;
        void Update() override;

        // Matrices
        const Matrix& GetViewMatrix()           const { return m_renderCamera->GetViewMatrix(); }
        const Matrix& GetProjectionMatrix()     const { return m_renderCamera->GetProjectionMatrix(); }
        const Matrix& GetViewProjectionMatrix() const { return m_renderCamera->GetViewProjectionMatrix(); }

        // Raycasting
        const Ray ComputePickingRay();
        const Ray& GetPickingRay() const { return m_ray; }

        // Picks the nearest entity under the mouse cursor
        void Pick();

        // Converts a world point to a screen point
        Vector2 WorldToScreenCoordinates(const Vector3& position_world) const;

        // Converts a world bounding box to a screen rectangle
        Rectangle WorldToScreenCoordinates(const BoundingBox& bounding_box) const;

        // Converts a screen point to a world point. Z can be 0.0f to 1.0f and it will lerp between the near and far plane.
        Vector3 ScreenToWorldCoordinates(const Vector2& position_screen, const float z) const;
        //=================================================================================================================

        // Aperture
        float GetAperture() const { return m_renderCamera->GetAperture(); }
        void SetAperture(const float aperture) { m_renderCamera->SetAperture(aperture); }

        // Shutter speed
        float GetShutterSpeed() const { return m_renderCamera->GetShutterSpeed(); }
        void SetShutterSpeed(const float shutter_speed) { m_renderCamera->SetShutterSpeed(shutter_speed); }

        // ISO
        float GetIso() const { return m_renderCamera->GetIso(); }
        void SetIso(const float iso) { m_renderCamera->SetIso(iso); }

        // Exposure
        float GetEv100()    const { return m_renderCamera->GetEv100(); }
        float GetExposure() const { return m_renderCamera->GetExposure(); }


        // Planes/projection
        void SetNearPlane(float near_plane);
        void SetFarPlane(float far_plane);
        void SetProjection(ProjectionType projection);
        float GetNearPlane()               const { return m_renderCamera->GetNearPlane(); }
        float GetFarPlane()                const { return m_renderCamera->GetFarPlane(); }
        ProjectionType GetProjectionType() const { return m_renderCamera->GetProjectionType(); }

        // FOV
        float GetFovHorizontalRad() const { return m_renderCamera->GetFovHorizontalRad(); }
        float GetFovVerticalRad()   const;
        float GetFovHorizontalDeg() const;
        void SetFovHorizontalDeg(float fov);

        // Frustum
        bool IsInViewFrustum(MeshFilter* renderable) const;
        bool IsInViewFrustum(const Vector3& center, const Vector3& extents) const;
        
        // Clear color
        const Color& GetClearColor()                   const { return m_renderCamera->GetClearColor(); }
        void SetClearColor(const Color& color) { m_renderCamera->SetClearColor(color); }

        // First person control
        bool GetFirstPersonControlEnabled()            const { return m_first_person_control_enabled; }
        void SetFirstPersonControlEnabled(const bool enabled) { m_first_person_control_enabled = enabled; }
        bool IsControledInFirstPerson() const;

        // Misc todo: will be remove
        void MakeDirty() { m_is_dirty = true; }
        void SetSelectedEntity(GameObject* entity) { m_selected_entity = entity; }
        GameObject* GetSelectedEntity() { return m_selected_entity; }

        void GoToCameraBookmark(int bookmark_index);
        void FocusOnSelectedEntity();

        RenderCamera* GetRenderCamera() { return m_renderCamera; }

    private:
        void ProcessInput();
        void ProcessInputFpsControl();
        void ProcessInputLerpToEntity();

        Vector3 m_position = Vector3::Zero;
        Quaternion m_rotation = Quaternion::Identity;

        bool m_is_dirty = false;
        bool m_first_person_control_enabled = true;
        bool m_is_controlled_by_keyboard_mouse = false;
        Vector2 m_mouse_last_position = Vector2::Zero;
        bool m_fps_control_cursor_hidden = false;
        Vector3 m_movement_speed = Vector3::Zero;
        float m_movement_scroll_accumulator = 0.0f;
        Vector2 m_mouse_smoothed = Vector2::Zero;
        Vector2 m_first_person_rotation = Vector2::Zero;
        float m_mouse_sensitivity = 0.2f;
        float m_mouse_smoothing = 0.5f;
        bool m_lerp_to_target_p = false;
        bool m_lerp_to_target_r = false;
        bool m_lerpt_to_bookmark = false;
        int m_target_bookmark_index = -1;
        float m_lerp_to_target_alpha = 0.0f;
        float m_lerp_to_target_distance = 0.0f;
        Vector3 m_lerp_to_target_position = Vector3::Zero;
        Quaternion m_lerp_to_target_rotation = Quaternion::Identity;
        RHI_Viewport m_last_known_viewport;

        Ray m_ray;
        GameObject* m_selected_entity = nullptr;

        RenderCamera* m_renderCamera;

        RTTR_ENABLE(Component)
    private:

        // todo Remove

        unsigned char m_depth;//排序深度

        unsigned char m_culling_mask;//控制渲染哪些Layer的物体
    };
}

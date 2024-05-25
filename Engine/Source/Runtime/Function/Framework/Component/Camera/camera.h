
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

    enum class ClearFlags
    {
        SolidColor,
        SkyBox,
        DontClear,
        DepthOnly
    };

    struct CameraLightDesc
    {
        float m_aperture = 2.8f;         // Aperture value in f-stop. Controls the amount of light, depth of field and chromatic aberration.
        float m_shutter_speed = 1.0f / 60.0f; // Length of time for which the camera shutter is open (sec). Also controls the amount of motion blur.
        float m_iso = 500.0f;       // Sensitivity to light.
    };

    struct CameraViewport
    {
        Vector2 m_pos;
        Vector2 m_size;
    };

    class Camera : public Component {
    public:
        Camera();
        ~Camera();
        
    public:
        void OnAwake() override;
        void OnUpdate() override;

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

        // Clear Flags
        ClearFlags GetClearFlags() { return m_clearFlags; }
        void SetClearFlags(ClearFlags clearFlags) { m_clearFlags = clearFlags; }

        // Clear color
        const Color& GetClearColor()                   const { return m_clearColor; }
        void SetClearColor(const Color& color) { m_clearColor = color; }

        // LightDesc
        CameraLightDesc GetCameraLightDesc() { return m_cameraLightDesc; }
        void SetCameraLightDesc(CameraLightDesc cameraLightDesc) { m_cameraLightDesc = cameraLightDesc; }
        float GetEv100()    const { return m_renderCamera->GetEv100(); }
        float GetExposure() const { return m_renderCamera->GetExposure(); }

        // Planes/projection
        void SetProjectionType(ProjectionType projection) { m_projection_type = projection; }
        ProjectionType GetProjectionType() const { return m_projection_type; }
        void SetNearPlane(float near_plane) { m_near_plane = near_plane; }
        float GetNearPlane()               const { return m_near_plane; }
        void SetFarPlane(float far_plane) { m_far_plane = far_plane; }
        float GetFarPlane()                const { return m_far_plane; }

        // FOV
        void SetFovHorizontal(float fov) { m_fov_horizontal = fov; }
        float GetFovHorizontal() const { return m_fov_horizontal; }

        void SetViewport(CameraViewport viewport) { m_cameraViewport = viewport; }
        CameraViewport GetViewport() { return m_cameraViewport; }

        void SetDepth(unsigned char depth) { m_depth = depth; }
        unsigned char GetDepth() { return m_depth; }

        // Frustum
        bool IsInViewFrustum(MeshFilter* renderable) const;
        bool IsInViewFrustum(const Vector3& center, const Vector3& extents) const;
        
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

    public:
        void PostResourceLoaded() override;
        void PostResourceModify() override;
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

        ClearFlags m_clearFlags = ClearFlags::SolidColor;
        Color m_clearColor;
        ProjectionType m_projection_type = Projection_Perspective;
        float m_near_plane = 0.1f;
        float m_far_plane = 4000.0f;
        float m_fov_horizontal = 90.0f;
        CameraLightDesc m_cameraLightDesc;
        CameraViewport m_cameraViewport{Vector2::Zero,Vector2(1920,1080)};

        unsigned char m_depth;// multi camera sort flag, smaller values are rendering first

        unsigned char m_culling_mask;// control what game object draw
    };
}

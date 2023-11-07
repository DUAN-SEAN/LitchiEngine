
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
#include "Runtime/Function/Renderer/Rendering/Color.h"
#include "Runtime/Function/Renderer/RHI/RHI_Viewport.h"

namespace LitchiRuntime
{
	class MeshFilter;

	enum ProjectionType
    {
        Projection_Perspective,
        Projection_Orthographic,
    };

    struct camera_bookmark
    {
        Vector3 position = Vector3::Zero;
        Vector3 rotation = Vector3::Zero;
    };

    class Camera : public Component {
    public:
        Camera();
        ~Camera();
        
    public:
        void Awake() override;
        void Update() override;

        // Matrices
        const Matrix& GetViewMatrix()           const { return m_view; }
        const Matrix& GetProjectionMatrix()     const { return m_projection; }
        const Matrix& GetViewProjectionMatrix() const { return m_view_projection; }

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
        float GetAperture() const { return m_aperture; }
        void SetAperture(const float aperture) { m_aperture = aperture; }

        // Shutter speed
        float GetShutterSpeed() const { return m_shutter_speed; }
        void SetShutterSpeed(const float shutter_speed) { m_shutter_speed = shutter_speed; }

        // ISO
        float GetIso() const { return m_iso; }
        void SetIso(const float iso) { m_iso = iso; }

        // Exposure
        float GetEv100()    const { return std::log2(m_aperture / m_shutter_speed * 100.0f / m_iso); }
        float GetExposure() const { return 1.0f / (std::pow(2.0f, GetEv100())); }


        // Planes/projection
        void SetNearPlane(float near_plane);
        void SetFarPlane(float far_plane);
        void SetProjection(ProjectionType projection);
        float GetNearPlane()               const { return m_near_plane; }
        float GetFarPlane()                const { return m_far_plane; }
        ProjectionType GetProjectionType() const { return m_projection_type; }

        // FOV
        float GetFovHorizontalRad() const { return m_fov_horizontal_rad; }
        float GetFovVerticalRad()   const;
        float GetFovHorizontalDeg() const;
        void SetFovHorizontalDeg(float fov);

        // Frustum
        bool IsInViewFrustum(MeshFilter* renderable) const;
        bool IsInViewFrustum(const Vector3& center, const Vector3& extents) const;

        // Bookmarks
        void AddBookmark(camera_bookmark bookmark) { m_bookmarks.emplace_back(bookmark); };
        const std::vector<camera_bookmark>& GetBookmarks() const { return m_bookmarks; };

        // Clear color
        const Color& GetClearColor()                   const { return m_clear_color; }
        void SetClearColor(const Color& color) { m_clear_color = color; }

        // First person control
        bool GetFirstPersonControlEnabled()            const { return m_first_person_control_enabled; }
        void SetFirstPersonControlEnabled(const bool enabled) { m_first_person_control_enabled = enabled; }
        bool IsControledInFirstPerson() const;

        // Misc
        void MakeDirty() { m_is_dirty = true; }
        void SetSelectedEntity(GameObject* entity) { m_selected_entity = entity; }
        GameObject* GetSelectedEntity() { return m_selected_entity; }

        Matrix ComputeViewMatrix() const;
        Matrix ComputeProjection(const float near_plane, const float far_plane);

        void GoToCameraBookmark(int bookmark_index);
        void FocusOnSelectedEntity();

    private:
        void ProcessInput();
        void ProcessInputFpsControl();
        void ProcessInputLerpToEntity();

        float m_aperture = 2.8f;         // Aperture value in f-stop. Controls the amount of light, depth of field and chromatic aberration.
        float m_shutter_speed = 1.0f / 60.0f; // Length of time for which the camera shutter is open (sec). Also controls the amount of motion blur.
        float m_iso = 500.0f;       // Sensitivity to light.
        float m_fov_horizontal_rad = Math::Helper::DegreesToRadians(90.0f);
        float m_near_plane = 0.1f;
        float m_far_plane = 1000.0f;
        ProjectionType m_projection_type = Projection_Perspective;
        Color m_clear_color = Color::standard_cornflower_blue;
        Matrix m_view = Matrix::Identity;
        Matrix m_projection = Matrix::Identity;
        Matrix m_view_projection = Matrix::Identity;
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
        Frustum m_frustum;
        std::vector<camera_bookmark> m_bookmarks;
        GameObject* m_selected_entity = nullptr;

        RTTR_ENABLE(Component)
    private:

        // todo Remove

        unsigned char m_depth;//排序深度

        unsigned char m_culling_mask;//控制渲染哪些Layer的物体
    };
}

#pragma once


#include <algorithm>

#include "RHI/RHI_Viewport.h"
#include "Runtime/Core/Math/Frustum.h"
#include "Runtime/Core/Math/Ray.h"
#include "Runtime/Core/Math/Rectangle.h"
#include "Runtime/Core/Math/Vector2.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"

namespace LitchiRuntime
{
	/**
    * Projection modes, mostly used for cameras
    */
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

	class RenderCamera
	{
	public:

        RenderCamera();
        ~RenderCamera();
		
    public:

        void Initialize();
        void Update();

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
        bool IsInViewFrustum(const BoundingBox& bounding_box) const;
        bool IsInViewFrustum(const Vector3& center, const Vector3& extents) const;
        
        // Clear color
        const Color& GetClearColor()                   const { return m_clear_color; }
        void SetClearColor(const Color& color) { m_clear_color = color; }
        
        void SetSelectedEntity(GameObject* entity) { m_selected_entity = entity; }
        GameObject* GetSelectedEntity() { return m_selected_entity; }

        Matrix ComputeViewMatrix() const;
        Matrix ComputeProjection(const float near_plane, const float far_plane);
        
		const RHI_Viewport& GetViewport();
		void SetViewport(float width, float height);

        //= POSITION ======================================================================
        void SetPosition(const Vector3& pos) { m_position = pos; }
        Vector3 GetPosition()             const { return m_position; }
        //=================================================================================

        //= ROTATION ======================================================================
        void SetRotation(const Quaternion& rotation) { m_rotation = rotation; }
        Quaternion GetRotation()             const { return m_rotation; }

        Vector3 GetUp()       const;
        Vector3 GetDown()     const;
        Vector3 GetForward()  const;
        Vector3 GetBackward() const;
        Vector3 GetRight()    const;
        Vector3 GetLeft()     const;

    private:

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

        // world pos,rotation
        Vector3 m_position = Vector3::Zero;
        Quaternion m_rotation = Quaternion::Identity;
        
        RHI_Viewport m_last_known_viewport;
        Ray m_ray;
        Frustum m_frustum;
        GameObject* m_selected_entity = nullptr;
        RHI_Viewport m_viewport = RHI_Viewport(0, 0, 0, 0);
	};
}

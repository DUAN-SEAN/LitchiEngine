
#include "RenderCamera.h"

#include "Rendering/Renderer.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

namespace LitchiRuntime
{
	RenderCamera::RenderCamera()
	{

	}

    RenderCamera::~RenderCamera()
    {
    }

    void RenderCamera::Initialize()
    {
		// default viewport
        auto& size = ApplicationBase::Instance()->window->GetSize();
        uint32_t width = size.first;
        uint32_t height = size.second;

        SetViewport(width, height);

        m_view = ComputeViewMatrix();
        m_projection = ComputeProjection(m_far_plane, m_near_plane); // reverse-z
        m_view_projection = m_view * m_projection;
    }

    void RenderCamera::Update()
    {
        m_view = ComputeViewMatrix();
        m_projection = ComputeProjection(m_far_plane, m_near_plane); // reverse-z
        m_view_projection = m_view * m_projection;
        m_frustum = Frustum(GetViewMatrix(), GetProjectionMatrix(), m_near_plane); // reverse-z
    }


    void RenderCamera::SetNearPlane(const float near_plane)
    {
        float near_plane_limited = Math::Helper::Max(near_plane, 0.01f);

        if (m_near_plane != near_plane_limited)
        {
            m_near_plane = near_plane_limited;
        }
    }

    void RenderCamera::SetFarPlane(const float far_plane)
    {
        m_far_plane = far_plane;
    }

    void RenderCamera::SetProjection(const ProjectionType projection)
    {
        m_projection_type = projection;
    }

    float RenderCamera::GetFovHorizontalDeg() const
    {
        return Math::Helper::RadiansToDegrees(m_fov_horizontal_rad);
    }

    float RenderCamera::GetFovVerticalRad() const
    {
        return 2.0f * atan(tan(m_fov_horizontal_rad / 2.0f) * (m_viewport.height / m_viewport.width));
    }

    void RenderCamera::SetFovHorizontalDeg(const float fov)
    {
        m_fov_horizontal_rad = Math::Helper::DegreesToRadians(fov);
    }

    bool RenderCamera::IsInViewFrustum(MeshFilter* renderable) const
    {
        EASY_FUNCTION(profiler::colors::Blue600)
        const BoundingBox& box = renderable->GetAAbb();
        const Vector3 center = box.GetCenter();
        const Vector3 extents = box.GetExtents();

        return m_frustum.IsVisible(center, extents);
    }

    bool RenderCamera::IsInViewFrustum(const Vector3& center, const Vector3& extents) const
    {
        EASY_FUNCTION(profiler::colors::Blue600)
        return m_frustum.IsVisible(center, extents);
    }

    const Ray RenderCamera::ComputePickingRay()
    {
        //Vector3 ray_start = GetGameObject()->GetComponent<Transform>()->GetPosition();
        Vector3 ray_start = m_position;
        Vector3 ray_direction = ScreenToWorldCoordinates(InputManager::GetMousePositionRelativeToEditorViewport(), 1.0f);
        return Ray(ray_start, ray_direction);
    }

    void RenderCamera::Pick()
    {
        // Ensure the mouse is inside the viewport
        if (!InputManager::GetMouseIsInViewport())
        {
            m_selected_entity = nullptr;
            return;
        }

        m_ray = ComputePickingRay();

        // Traces ray against all AABBs in the world
        std::vector<RayHit> hits;
        {
            const auto& entities = ApplicationBase::Instance()->sceneManager->GetCurrentScene()->GetAllGameObjectList();
            for (const auto entity : entities)
            {
                // Make sure there entity has a renderable
                if (!entity->GetComponent<MeshFilter>())
                    continue;

                // Get object oriented bounding box
                const BoundingBox& aabb = entity->GetComponent<MeshFilter>()->GetAAbb();

                // Compute hit distance
                float distance = m_ray.HitDistance(aabb);

                // Don't store hit data if there was no hit
                if (distance == Math::Helper::INFINITY_)
                    continue;

                hits.emplace_back(
                    entity,                                             // Entity
                    m_ray.GetStart() + m_ray.GetDirection() * distance, // Position
                    distance,                                           // Distance
                    distance == 0.0f                                    // Inside
                );
            }

            // Sort by distance (ascending)
            sort(hits.begin(), hits.end(), [](const RayHit& a, const RayHit& b) { return a.m_distance < b.m_distance; });
        }

        // Check if there are any hits
        if (hits.empty())
        {
            m_selected_entity = nullptr;
            return;
        }

        // If there is a single hit, return that
        if (hits.size() >= 1)
        {
            m_selected_entity = hits.front().m_entity;
            return;
        }

        //// If there are more hits, perform triangle intersection
        //float distance_min = std::numeric_limits<float>::max();
        //for (RayHit& hit : hits)
        //{
        //    // Get entity geometry
        //    auto renderable = hit.m_entity->GetComponent<MeshFilter>();
        //    std::vector<uint32_t> indicies;
        //    std::vector<RHI_Vertex_PosTexNorTan> vertices;
        //    renderable->GetGeometry(&indicies, &vertices);
        //    if (indicies.empty() || vertices.empty())
        //    {
        //        DEBUG_LOG_ERROR("Failed to get geometry of entity %s, skipping intersection test.");
        //        continue;
        //    }

        //    // Compute matrix which can transform vertices to view space
        //    Matrix vertex_transform = hit.m_entity->GetComponent<Transform>()->GetMatrix();

        //    // Go through each face
        //    for (uint32_t i = 0; i < indicies.size(); i += 3)
        //    {
        //        Vector3 p1_world = Vector3(vertices[indicies[i]].pos) * vertex_transform;
        //        Vector3 p2_world = Vector3(vertices[indicies[i + 1]].pos) * vertex_transform;
        //        Vector3 p3_world = Vector3(vertices[indicies[i + 2]].pos) * vertex_transform;

        //        float distance = m_ray.HitDistance(p1_world, p2_world, p3_world);

        //        if (distance < distance_min)
        //        {
        //            m_selected_entity = hit.m_entity;
        //            distance_min = distance;
        //        }
        //    }
        //}
    }

    Vector2 RenderCamera::WorldToScreenCoordinates(const Vector3& position_world) const
    {
        const RHI_Viewport& viewport = m_viewport;

        // A non reverse-z projection matrix is need, we create it
        const Matrix projection = Matrix::CreatePerspectiveFieldOfViewLH(GetFovVerticalRad(), viewport.GetAspectRatio(), m_near_plane, m_far_plane);

        // Convert world space position to clip space position
        const Vector3 position_clip = position_world * m_view * projection;

        // Convert clip space position to screen space position
        Vector2 position_screen;
        position_screen.x = (position_clip.x / position_clip.z) * (0.5f * viewport.width) + (0.5f * viewport.width);
        position_screen.y = (position_clip.y / position_clip.z) * -(0.5f * viewport.height) + (0.5f * viewport.height);

        return position_screen;
    }

    Rectangle RenderCamera::WorldToScreenCoordinates(const BoundingBox& bounding_box) const
    {
        const Vector3& min = bounding_box.GetMin();
        const Vector3& max = bounding_box.GetMax();

        Vector3 corners[8];
        corners[0] = min;
        corners[1] = Vector3(max.x, min.y, min.z);
        corners[2] = Vector3(min.x, max.y, min.z);
        corners[3] = Vector3(max.x, max.y, min.z);
        corners[4] = Vector3(min.x, min.y, max.z);
        corners[5] = Vector3(max.x, min.y, max.z);
        corners[6] = Vector3(min.x, max.y, max.z);
        corners[7] = max;

        Rectangle rectangle;
        for (Vector3& corner : corners)
        {
            rectangle.Merge(WorldToScreenCoordinates(corner));
        }

        return rectangle;
    }

    Vector3 RenderCamera::ScreenToWorldCoordinates(const Vector2& position_screen, const float z) const
    {
        DEBUG_LOG_INFO("positionScreen {},{}",position_screen.x, position_screen.y);

        const RHI_Viewport& viewport = m_viewport;

        // A non reverse-z projection matrix is need, we create it
        const Matrix projection = Matrix::CreatePerspectiveFieldOfViewLH(GetFovVerticalRad(), viewport.GetAspectRatio(), m_near_plane, m_far_plane); // reverse-z

        // Convert screen space position to clip space position
        Vector3 position_clip;
        position_clip.x = (position_screen.x / viewport.width) * 2.0f - 1.0f;
        position_clip.y = (position_screen.y / viewport.height) * -2.0f + 1.0f;
        position_clip.z = std::clamp(z, 0.0f, 1.0f);

        // Compute world space position
        Matrix view_projection_inverted = (m_view * projection).Inverted();
        Vector4 position_world = Vector4(position_clip, 1.0f) * view_projection_inverted;

        return Vector3(position_world) / position_world.w;
    }
    
    const RHI_Viewport& RenderCamera::GetViewport()
    {
        return m_viewport;
    }

    void RenderCamera::SetViewport(float width, float height)
    {
        m_viewport.width = width;
        m_viewport.height = height;
    }
    
    Matrix RenderCamera::ComputeViewMatrix() const
    {
        const auto position = m_position;
        auto look_at = GetForward();
        const auto up = GetUp();

        // offset look_at by current position
        look_at += position;

        // compute view matrix
        return Matrix::CreateLookAtLH(position, look_at, up);
    }

    Matrix RenderCamera::ComputeProjection(const float near_plane, const float far_plane)
    {
        if (m_projection_type == Projection_Perspective)
        {
            return Matrix::CreatePerspectiveFieldOfViewLH(GetFovVerticalRad(), m_viewport.GetAspectRatio(), near_plane, far_plane);
        }
        else if (m_projection_type == Projection_Orthographic)
        {
            return Matrix::CreateOrthographicLH(m_viewport.width, m_viewport.height, near_plane, far_plane);
        }

        return Matrix::Identity;
    }
    
    Vector3 RenderCamera::GetUp() const
    {
        return m_rotation * Vector3::Up;
    }

    Vector3 RenderCamera::GetDown() const
    {
        return m_rotation * Vector3::Down;
    }

    Vector3 RenderCamera::GetForward() const
    {
        return m_rotation * Vector3::Forward;
    }

    Vector3 RenderCamera::GetBackward() const
    {
        return m_rotation * Vector3::Backward;
    }

    Vector3 RenderCamera::GetRight() const
    {
        return m_rotation * Vector3::Right;
    }

    Vector3 RenderCamera::GetLeft() const
    {
        return m_rotation * Vector3::Left;
    }

}


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


    void RenderCamera::SetNearPlane(const float near_plane)
    {
        float near_plane_limited = Math::Helper::Max(near_plane, 0.01f);

        if (m_near_plane != near_plane_limited)
        {
            m_near_plane = near_plane_limited;
            m_is_dirty = true;
        }
    }

    void RenderCamera::SetFarPlane(const float far_plane)
    {
        m_far_plane = far_plane;
        m_is_dirty = true;
    }

    void RenderCamera::SetProjection(const ProjectionType projection)
    {
        m_projection_type = projection;
        m_is_dirty = true;
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
        m_is_dirty = true;
    }

    bool RenderCamera::IsInViewFrustum(MeshFilter* renderable) const
    {
        const BoundingBox& box = renderable->GetAabb();
        const Vector3 center = box.GetCenter();
        const Vector3 extents = box.GetExtents();

        return m_frustum.IsVisible(center, extents);
    }

    bool RenderCamera::IsInViewFrustum(const Vector3& center, const Vector3& extents) const
    {
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
                const BoundingBox& aabb = entity->GetComponent<MeshFilter>()->GetAabb();

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
        if (hits.size() == 1)
        {
            m_selected_entity = hits.front().m_entity;
            return;
        }

        // If there are more hits, perform triangle intersection
        float distance_min = std::numeric_limits<float>::max();
        for (RayHit& hit : hits)
        {
            // Get entity geometry
            auto renderable = hit.m_entity->GetComponent<MeshFilter>();
            std::vector<uint32_t> indicies;
            std::vector<RHI_Vertex_PosTexNorTan> vertices;
            renderable->GetGeometry(&indicies, &vertices);
            if (indicies.empty() || vertices.empty())
            {
                DEBUG_LOG_ERROR("Failed to get geometry of entity %s, skipping intersection test.");
                continue;
            }

            // Compute matrix which can transform vertices to view space
            Matrix vertex_transform = hit.m_entity->GetComponent<Transform>()->GetMatrix();

            // Go through each face
            for (uint32_t i = 0; i < indicies.size(); i += 3)
            {
                Vector3 p1_world = Vector3(vertices[indicies[i]].pos) * vertex_transform;
                Vector3 p2_world = Vector3(vertices[indicies[i + 1]].pos) * vertex_transform;
                Vector3 p3_world = Vector3(vertices[indicies[i + 2]].pos) * vertex_transform;

                float distance = m_ray.HitDistance(p1_world, p2_world, p3_world);

                if (distance < distance_min)
                {
                    m_selected_entity = hit.m_entity;
                    distance_min = distance;
                }
            }
        }
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

    void RenderCamera::ProcessInput()
    {
        // FPS camera controls.
        // X-axis movement: W, A, S, D.
        // Y-axis movement: Q, E.
        // Mouse look: Hold right click to enable.
        if (m_first_person_control_enabled)
        {
            ProcessInputFpsControl();
        }

        // Shortcuts
        {
            // Focus on selected entity: F.
            ProcessInputLerpToEntity();
        }
    }

    void RenderCamera::ProcessInputFpsControl()
    {
        static const float movement_speed_max = 5.0f;
        static float movement_acceleration = 1.0f;
        static const float movement_drag = 10.0f;
        Vector3 movement_direction = Vector3::Zero;
        float delta_time = static_cast<float>(Time::delta_time());

        // Detect if fps control should be activated
        {
            // Initiate control only when the mouse is within the viewport
            if (InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_RIGHT) == EMouseButtonState::MOUSE_DOWN && InputManager::GetMouseIsInViewport())
            {
                m_is_controlled_by_keyboard_mouse = true;
            }

            // Maintain control as long as the right click is pressed and initial control has been given
            m_is_controlled_by_keyboard_mouse = InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_RIGHT) == EMouseButtonState::MOUSE_DOWN && m_is_controlled_by_keyboard_mouse;
        }

        // Cursor visibility and position
        {
            // Toggle mouse cursor and adjust mouse position
            if (m_is_controlled_by_keyboard_mouse && !m_fps_control_cursor_hidden)
            {
                m_mouse_last_position = InputManager::GetMousePosition();

                if (!ApplicationBase::Instance()->window->IsFullscreen())
                    // if (!  Window::IsFullscreen()) // change the mouse state only in editor mode
                {
                    // todo:
                    // InputManager::SetMouseCursorVisible(false);
                }

                m_fps_control_cursor_hidden = true;
            }
            else if (!m_is_controlled_by_keyboard_mouse && m_fps_control_cursor_hidden)
            {
                InputManager::SetMousePosition(m_mouse_last_position);

                // if (!Window::IsFullscreen()) // change the mouse state only in editor mode
                if (!ApplicationBase::Instance()->window->IsFullscreen()) // change the mouse state only in editor mode
                {
                    // todo:
                    // InputManager::SetMouseCursorVisible(true);
                }

                m_fps_control_cursor_hidden = false;
            }
        }

        if (m_is_controlled_by_keyboard_mouse)
        {
            // Mouse look
            {
                // Wrap around left and right screen edges (to allow for infinite scrolling)
                {
                    uint32_t edge_padding = 5;
                    Vector2 mouse_position = InputManager::GetMousePosition();
                    if (mouse_position.x >= ApplicationBase::Instance()->window->GetWidth() - edge_padding)
                    {
                        mouse_position.x = static_cast<float>(edge_padding + 1);
                        InputManager::SetMousePosition(mouse_position);
                    }
                    else if (mouse_position.x <= edge_padding)
                    {
                        mouse_position.x = static_cast<float>(ApplicationBase::Instance()->window->GetWidth() - edge_padding - 1);
                        InputManager::SetMousePosition(mouse_position);
                    }
                }

                // Get camera rotation.
              /*  m_first_person_rotation.x = GetGameObject()->GetComponent<Transform>()->GetRotation().Yaw();
                m_first_person_rotation.y = GetGameObject()->GetComponent<Transform>()->GetRotation().Pitch();*/
                m_first_person_rotation.x = m_rotation.Yaw();
                m_first_person_rotation.y = m_rotation.Pitch();

                // Get mouse delta.
                const Vector2 mouse_delta = InputManager::GetMouseDelta() * m_mouse_sensitivity;

                // Lerp to it.
                m_mouse_smoothed = Math::Helper::Lerp(m_mouse_smoothed, mouse_delta, Math::Helper::Saturate(1.0f - m_mouse_smoothing));

                // Accumulate rotation.
                m_first_person_rotation += m_mouse_smoothed;

                // Clamp rotation along the x-axis (but not exactly at 90 degrees, this is to avoid a gimbal lock).
                m_first_person_rotation.y = Math::Helper::Clamp(m_first_person_rotation.y, -80.0f, 80.0f);

                // Compute rotation.
                const Quaternion xQuaternion = Quaternion::FromAngleAxis(m_first_person_rotation.x * Math::Helper::DEG_TO_RAD, Vector3::Up);
                const Quaternion yQuaternion = Quaternion::FromAngleAxis(m_first_person_rotation.y * Math::Helper::DEG_TO_RAD, Vector3::Right);
                const Quaternion rotation = xQuaternion * yQuaternion;

                // Rotate
                //GetGameObject()->GetComponent<Transform>()->SetRotationLocal(rotation);
                m_rotation = rotation;
            }

            // Keyboard movement direction
            {
                // Compute direction
                if (InputManager::GetKeyState(EKey::KEY_W) == EKeyState::KEY_DOWN) movement_direction += GetForward();
                if (InputManager::GetKeyState(EKey::KEY_S) == EKeyState::KEY_DOWN) movement_direction += GetBackward();
                if (InputManager::GetKeyState(EKey::KEY_D) == EKeyState::KEY_DOWN) movement_direction += GetRight();
                if (InputManager::GetKeyState(EKey::KEY_A) == EKeyState::KEY_DOWN) movement_direction += GetLeft();
                if (InputManager::GetKeyState(EKey::KEY_Q) == EKeyState::KEY_DOWN) movement_direction += GetDown();
                if (InputManager::GetKeyState(EKey::KEY_E) == EKeyState::KEY_DOWN) movement_direction += GetUp();
                movement_direction.Normalize();
            }

            // Wheel delta (used to adjust movement speed)
            {
                // Accumulate
                m_movement_scroll_accumulator += InputManager::GetMouseWheelDelta().y * 0.1f;

                // Clamp
                float min = -movement_acceleration + 0.1f; // Prevent it from negating or zeroing the acceleration, see translation calculation.
                float max = movement_acceleration * 2.0f;  // An empirically chosen max.
                m_movement_scroll_accumulator = Math::Helper::Clamp(m_movement_scroll_accumulator, min, max);
            }
        }

        // Controller movement
        // if (InputManager::IsControllerConnected())
        if (m_is_controlled_by_keyboard_mouse)
        {
            // Look
            {
                // Get camera rotation
                m_first_person_rotation.x += InputManager::GetMouseDelta().x;
                m_first_person_rotation.y += InputManager::GetMouseDelta().y;

                // Get mouse delta.
                const Vector2 mouse_delta = InputManager::GetMouseDelta() * m_mouse_sensitivity;

                // Clamp rotation along the x-axis (but not exactly at 90 degrees, this is to avoid a gimbal lock).
                m_first_person_rotation.y = Math::Helper::Clamp(m_first_person_rotation.y, -80.0f, 80.0f);

                // Compute rotation.
                const Quaternion xQuaternion = Quaternion::FromAngleAxis(m_first_person_rotation.x * Math::Helper::DEG_TO_RAD, Vector3::Up);
                const Quaternion yQuaternion = Quaternion::FromAngleAxis(m_first_person_rotation.y * Math::Helper::DEG_TO_RAD, Vector3::Right);
                const Quaternion rotation = xQuaternion * yQuaternion;

                // Rotate
                // GetGameObject()->GetComponent<Transform>()->SetRotationLocal(rotation);
                m_rotation = rotation;
            }

            // Controller movement direction
            movement_direction += GetForward() * -InputManager::GetMouseDelta().x;
            movement_direction += GetRight() * InputManager::GetMouseDelta().x;
            movement_direction += GetDown() * InputManager::GetMouseDelta().y;
            movement_direction += GetUp() * InputManager::GetMouseDelta().y;
            movement_direction.Normalize();
        }

        // Translation
        {
            Vector3 translation = (movement_acceleration + m_movement_scroll_accumulator) * movement_direction;

            // On shift, double the translation
            if (InputManager::GetKeyState(EKey::KEY_LEFT_ALT) == EKeyState::KEY_DOWN)
            {
                translation *= 2.0f;
            }

            // Accelerate
            m_movement_speed += translation * delta_time;

            // Apply drag
            m_movement_speed *= 1.0f - movement_drag * delta_time;

            // Clamp it
            if (m_movement_speed.Length() > movement_speed_max)
            {
                m_movement_speed = m_movement_speed.Normalized() * movement_speed_max;
            }

            // Translate for as long as there is speed
            if (m_movement_speed != Vector3::Zero)
            {
                // GetGameObject()->GetComponent<Transform>()->Translate(m_movement_speed);
                m_position += m_movement_speed;
            }
        }
    }

    void RenderCamera::ProcessInputLerpToEntity()
    {
        // Set focused entity as a lerp target
        if (InputManager::GetKeyState(EKey::KEY_F) == EKeyState::KEY_DOWN)
        {
            FocusOnSelectedEntity();
        }

        // Set bookmark as a lerp target
        bool lerp_to_bookmark = false;
        if (lerp_to_bookmark = m_lerpt_to_bookmark && m_target_bookmark_index >= 0 && m_target_bookmark_index < m_bookmarks.size())
        {
            m_lerp_to_target_position = m_bookmarks[m_target_bookmark_index].position;

            // Compute lerp speed based on how far the entity is from the camera.
            //m_lerp_to_target_distance = Vector3::Distance(m_lerp_to_target_position, GetGameObject()->GetComponent<Transform>()->GetPosition());
            m_lerp_to_target_distance = Vector3::Distance(m_lerp_to_target_position, m_position);
            m_lerp_to_target_p = true;

            m_target_bookmark_index = -1;
            m_lerpt_to_bookmark = false;
        }

        // Lerp
        if (m_lerp_to_target_p || m_lerp_to_target_r || lerp_to_bookmark)
        {
            // Lerp duration in seconds
            // 2.0 seconds + [0.0 - 2.0] seconds based on distance
            // Something is not right with the duration...
            const float lerp_duration = 2.0f + Math::Helper::Clamp(m_lerp_to_target_distance * 0.01f, 0.0f, 2.0f);

            // Alpha
            m_lerp_to_target_alpha += static_cast<float>(Time::delta_time()) / lerp_duration;

            // Position
            if (m_lerp_to_target_p)
            {
                const Vector3 interpolated_position = Vector3::Lerp(m_position, m_lerp_to_target_position, m_lerp_to_target_alpha);
                // GetGameObject()->GetComponent<Transform>()->SetPosition(interpolated_position);
                m_position = (interpolated_position);
            }

            // Rotation
            if (m_lerp_to_target_r)
            {
                const Quaternion interpolated_rotation = Quaternion::Lerp(m_rotation, m_lerp_to_target_rotation, Math::Helper::Clamp(m_lerp_to_target_alpha, 0.0f, 1.0f));
                m_rotation = (interpolated_rotation);
            }

            // If the lerp has completed or the user has initiated fps control, stop lerping.
            if (m_lerp_to_target_alpha >= 1.0f || m_is_controlled_by_keyboard_mouse)
            {
                m_lerp_to_target_p = false;
                m_lerp_to_target_r = false;
                m_lerp_to_target_alpha = 0.0f;
                m_lerp_to_target_position = Vector3::Zero;
            }
        }
    }

    void RenderCamera::FocusOnSelectedEntity()
    {
        if (GameObject* entity = Renderer::GetCamera()->GetSelectedEntity())
        {
            DEBUG_LOG_INFO("Focusing on entity {}", entity->GetObjectName());

            m_lerp_to_target_position = entity->GetComponent<Transform>()->GetPosition();
            const Vector3 target_direction = (m_lerp_to_target_position - m_position).Normalized();

            // If the entity has a renderable component, we can get a more accurate target position.
            // ...otherwise we apply a simple offset so that the rotation vector doesn't suffer
            if (auto renderable = entity->GetComponent<MeshFilter>())
            {
                m_lerp_to_target_position -= target_direction * renderable->GetAabb().GetExtents().Length() * 2.0f;
            }
            else
            {
                m_lerp_to_target_position -= target_direction;
            }

            m_lerp_to_target_rotation = Quaternion::FromLookRotation(entity->GetComponent<Transform>()->GetPosition() - m_lerp_to_target_position).Normalized();
            m_lerp_to_target_distance = Vector3::Distance(m_lerp_to_target_position, m_position);

            const float lerp_angle = acosf(Quaternion::Dot(m_lerp_to_target_rotation.Normalized(), m_rotation.Normalized())) * Math::Helper::RAD_TO_DEG;

            m_lerp_to_target_p = m_lerp_to_target_distance > 0.1f ? true : false;
            m_lerp_to_target_r = lerp_angle > 1.0f ? true : false;
        }
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

    bool RenderCamera::IsControledInFirstPerson() const
    {
        return m_is_controlled_by_keyboard_mouse;
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

    void RenderCamera::GoToCameraBookmark(int bookmark_index)
    {
        if (bookmark_index >= 0)
        {
            m_target_bookmark_index = bookmark_index;
            m_lerpt_to_bookmark = true;
        }
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

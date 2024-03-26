#include "Runtime/Core/pch.h"
#include "camera.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Core/Time/time.h"
#include "Runtime/Core/Window/Window.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
#include "Runtime/Function/Scene/SceneManager.h"

namespace LitchiRuntime
{

	Camera::Camera() :m_depth(0), m_culling_mask(0x01) {
        m_renderCamera = new RenderCamera();
        m_renderCamera->Initialize();
	}
	
	Camera::~Camera() {
        delete m_renderCamera;
        m_renderCamera = nullptr;
	}

    void Camera::OnAwake()
    {
        // todo default select
        m_renderCamera->SetViewport(1920, 1080);
    }

    void Camera::OnUpdate()
    {
        // 默认editor状态不执行
        const auto& current_viewport = m_renderCamera->GetViewport();
        if (m_last_known_viewport != current_viewport)
        {
            m_last_known_viewport = current_viewport;
            m_is_dirty = true;
        }

        // DIRTY CHECK
        auto* transform = GetGameObject()->GetComponent<Transform>();
        if (m_position != transform->GetPosition() || m_rotation != transform->GetRotation())
        {
            m_position = transform->GetPosition();
            m_rotation = transform->GetRotation();

       /*     DEBUG_LOG_INFO("Camera::Update Pos:{},{},{}", m_position.x, m_position.y, m_position.z);
            DEBUG_LOG_INFO("Camera::Update Rotation:{},{},{}", m_rotation.ToEulerAngles().x, m_rotation.ToEulerAngles().y, m_rotation.ToEulerAngles().z);*/

            m_is_dirty = true;
        }

        ProcessInput();

        if (!m_is_dirty)
            return;

        // update renderCamera vp matrix
        m_renderCamera->SetPosition(m_position);
        m_renderCamera->SetRotation(m_rotation);
        m_renderCamera->Update();

        m_is_dirty = false;
    }
	

    void Camera::SetNearPlane(const float near_plane)
    {
        m_renderCamera->SetNearPlane(near_plane);
        m_is_dirty = true;
    }

    void Camera::SetFarPlane(const float far_plane)
    {
        m_renderCamera->SetFarPlane(far_plane);
        m_is_dirty = true;
    }

    void Camera::SetProjection(const ProjectionType projection)
    {
        m_renderCamera->SetProjection(projection);
        m_is_dirty = true;
    }

    float Camera::GetFovHorizontalDeg() const
    {
        return m_renderCamera->GetFovHorizontalDeg();
    }

    float Camera::GetFovVerticalRad() const
    {
        return m_renderCamera->GetFovVerticalRad();
    }

    void Camera::SetFovHorizontalDeg(const float fov)
    {
        m_is_dirty = true;
        return m_renderCamera->SetFovHorizontalDeg(fov);
    }

    bool Camera::IsInViewFrustum(MeshFilter* renderable) const
    {
        const BoundingBox& box = renderable->GetAAbb();
        const Vector3 center = box.GetCenter();
        const Vector3 extents = box.GetExtents();

        return m_renderCamera->IsInViewFrustum(center, extents);
    }

    bool Camera::IsInViewFrustum(const Vector3& center, const Vector3& extents) const
    {
        return m_renderCamera->IsInViewFrustum(center, extents);
    }

    const Ray Camera::ComputePickingRay()
    {
        Vector3 ray_start = GetGameObject()->GetComponent<Transform>()->GetPosition();
        Vector3 ray_direction = ScreenToWorldCoordinates(InputManager::GetMousePositionRelativeToEditorViewport(), 1.0f);
        return Ray(ray_start, ray_direction);
    }

    void Camera::Pick()
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

    Vector2 Camera::WorldToScreenCoordinates(const Vector3& position_world) const
    {
        return m_renderCamera->WorldToScreenCoordinates(position_world);
    }

    Rectangle Camera::WorldToScreenCoordinates(const BoundingBox& bounding_box) const
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

    Vector3 Camera::ScreenToWorldCoordinates(const Vector2& position_screen, const float z) const
    {
        return m_renderCamera->ScreenToWorldCoordinates(position_screen, z);
    }

    void Camera::ProcessInput()
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

    void Camera::ProcessInputFpsControl()
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

                if(!ApplicationBase::Instance()->window->IsFullscreen())
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
                m_first_person_rotation.x = GetGameObject()->GetComponent<Transform>()->GetRotation().Yaw();
                m_first_person_rotation.y = GetGameObject()->GetComponent<Transform>()->GetRotation().Pitch();

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
                GetGameObject()->GetComponent<Transform>()->SetRotationLocal(rotation);
            }

            // Keyboard movement direction
            {
                // Compute direction
                if (InputManager::GetKeyState(EKey::KEY_W) == EKeyState::KEY_DOWN) movement_direction += GetGameObject()->GetComponent<Transform>()->GetForward();
                if (InputManager::GetKeyState(EKey::KEY_S) == EKeyState::KEY_DOWN) movement_direction += GetGameObject()->GetComponent<Transform>()->GetBackward();
                if (InputManager::GetKeyState(EKey::KEY_D) == EKeyState::KEY_DOWN) movement_direction += GetGameObject()->GetComponent<Transform>()->GetRight();
                if (InputManager::GetKeyState(EKey::KEY_A) == EKeyState::KEY_DOWN) movement_direction += GetGameObject()->GetComponent<Transform>()->GetLeft();
                if (InputManager::GetKeyState(EKey::KEY_Q) == EKeyState::KEY_DOWN) movement_direction += GetGameObject()->GetComponent<Transform>()->GetDown();
                if (InputManager::GetKeyState(EKey::KEY_E) == EKeyState::KEY_DOWN) movement_direction += GetGameObject()->GetComponent<Transform>()->GetUp();
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
        if(m_is_controlled_by_keyboard_mouse)
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
                GetGameObject()->GetComponent<Transform>()->SetRotationLocal(rotation);
            }

            // Controller movement direction
            movement_direction += GetGameObject()->GetComponent<Transform>()->GetForward() * -InputManager::GetMouseDelta().x;
            movement_direction += GetGameObject()->GetComponent<Transform>()->GetRight() * InputManager::GetMouseDelta().x;
            movement_direction += GetGameObject()->GetComponent<Transform>()->GetDown() * InputManager::GetMouseDelta().y;
            movement_direction += GetGameObject()->GetComponent<Transform>()->GetUp() * InputManager::GetMouseDelta().y;
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
                GetGameObject()->GetComponent<Transform>()->Translate(m_movement_speed);
            }
        }
    }

    void Camera::ProcessInputLerpToEntity()
    {
        // Set focused entity as a lerp target
        if (InputManager::GetKeyState(EKey::KEY_F) == EKeyState::KEY_DOWN)
        {
            FocusOnSelectedEntity();
        }

        // Set bookmark as a lerp target
        //bool lerp_to_bookmark = false;
        //if (lerp_to_bookmark = m_lerpt_to_bookmark && m_target_bookmark_index >= 0 && m_target_bookmark_index < m_bookmarks.size())
        //{
        //    m_lerp_to_target_position = m_bookmarks[m_target_bookmark_index].position;

        //    // Compute lerp speed based on how far the entity is from the camera.
        //    m_lerp_to_target_distance = Vector3::Distance(m_lerp_to_target_position, GetGameObject()->GetComponent<Transform>()->GetPosition());
        //    m_lerp_to_target_p = true;

        //    m_target_bookmark_index = -1;
        //    m_lerpt_to_bookmark = false;
        //}

        // Lerp
        // if (m_lerp_to_target_p || m_lerp_to_target_r || lerp_to_bookmark)
        if (m_lerp_to_target_p || m_lerp_to_target_r)
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
                const Vector3 interpolated_position = Vector3::Lerp(GetGameObject()->GetComponent<Transform>()->GetPosition(), m_lerp_to_target_position, m_lerp_to_target_alpha);
                GetGameObject()->GetComponent<Transform>()->SetPosition(interpolated_position);
            }

            // Rotation
            if (m_lerp_to_target_r)
            {
                const Quaternion interpolated_rotation = Quaternion::Lerp(GetGameObject()->GetComponent<Transform>()->GetRotation(), m_lerp_to_target_rotation, Math::Helper::Clamp(m_lerp_to_target_alpha, 0.0f, 1.0f));
                GetGameObject()->GetComponent<Transform>()->SetRotation(interpolated_rotation);
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

    void Camera::FocusOnSelectedEntity()
    {
        if (GameObject* entity = m_renderCamera->GetSelectedEntity())
        {
            DEBUG_LOG_INFO("Focusing on entity {}", entity->GetObjectName());

            m_lerp_to_target_position = entity->GetComponent<Transform>()->GetPosition();
            const Vector3 target_direction = (m_lerp_to_target_position - GetGameObject()->GetComponent<Transform>()->GetPosition()).Normalized();

            // If the entity has a renderable component, we can get a more accurate target position.
            // ...otherwise we apply a simple offset so that the rotation vector doesn't suffer
            if (auto renderable = entity->GetComponent<MeshFilter>())
            {
                m_lerp_to_target_position -= target_direction * renderable->GetAAbb().GetExtents().Length() * 2.0f;
            }
            else
            {
                m_lerp_to_target_position -= target_direction;
            }

            m_lerp_to_target_rotation = Quaternion::FromLookRotation(entity->GetComponent<Transform>()->GetPosition() - m_lerp_to_target_position).Normalized();
            m_lerp_to_target_distance = Vector3::Distance(m_lerp_to_target_position, GetGameObject()->GetComponent<Transform>()->GetPosition());

            const float lerp_angle = acosf(Quaternion::Dot(m_lerp_to_target_rotation.Normalized(), GetGameObject()->GetComponent<Transform>()->GetRotation().Normalized())) * Math::Helper::RAD_TO_DEG;

            m_lerp_to_target_p = m_lerp_to_target_distance > 0.1f ? true : false;
            m_lerp_to_target_r = lerp_angle > 1.0f ? true : false;
        }
    }

    bool Camera::IsControledInFirstPerson() const
    {
        return m_is_controlled_by_keyboard_mouse;
    }
    
    void Camera::GoToCameraBookmark(int bookmark_index)
    {
        if (bookmark_index >= 0)
        {
            m_target_bookmark_index = bookmark_index;
            m_lerpt_to_bookmark = true;
        }
    }
}












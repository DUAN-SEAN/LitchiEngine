
#include "Runtime/Core/pch.h"
#include <algorithm>

#include "Editor/include/Core/CameraController.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

#include "Runtime/Core/Math/MathHelper.h"
#include "Runtime/Core/Time/time.h"

LitchiEditor::CameraController::CameraController
(
	AView& p_view,
	RenderCamera* p_camera,
	Vector3& p_position,
	Quaternion& p_rotation,
	bool p_enableFocusInputs
) :
	m_window(*ApplicationEditor::Instance()->window),
	m_view(p_view),
	m_camera(p_camera)
{
	// 设置相机默认的位置和姿态
	auto cameraPosition = Vector3(0.0f, 5.0f, -10.0f);
	
	auto cameraRotation = Quaternion::FromEulerAngles((Vector3(Math::Helper::DegreesToRadians(45.0f), Math::Helper::DegreesToRadians(0.0f), 0.0f)));

	m_camera->SetFovHorizontalDeg(60.0f);
	m_camera->SetPosition(cameraPosition);
	m_camera->SetRotation(cameraRotation);
}

float GetActorFocusDist(GameObject* p_actor)
{
	float distance = 4.0f;

	/*if (p_actor.IsActive())
	{
		if (auto pb = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalBox>())
		{
			distance = std::max(distance, std::max
			(
				std::max
				(
					pb->GetSize().x * p_actor.GetComponent<Transform>()->GetWorldScale().x,
					pb->GetSize().y * p_actor.GetComponent<Transform>()->GetWorldScale().y
				),
				pb->GetSize().z * p_actor.GetComponent<Transform>()->GetWorldScale().z
			) * 1.5f);
		}

		if (auto ps = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalSphere>())
		{
			distance = std::max(distance, std::max
			(
				std::max
				(
					ps->GetRadius() * p_actor.GetComponent<Transform>()->GetWorldScale().x,
					ps->GetRadius() * p_actor.GetComponent<Transform>()->GetWorldScale().y
				),
				ps->GetRadius() * p_actor.GetComponent<Transform>()->GetWorldScale().z
			) * 1.5f);
		}

		if (auto pc = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalCapsule>())
		{
			distance = std::max(distance, std::max
			(
				std::max
				(
					pc->GetRadius() * p_actor.GetComponent<Transform>()->GetWorldScale().x,
					pc->GetHeight() * p_actor.GetComponent<Transform>()->GetWorldScale().y
				),
				pc->GetRadius() * p_actor.GetComponent<Transform>()->GetWorldScale().z
			) * 1.5f);
		}

		if (auto modelRenderer = p_actor.GetComponent<MeshFilter>())
		{
			const bool hasCustomBoundingSphere = modelRenderer->GetFrustumBehaviour() == MeshFilter::EFrustumBehaviour::CULL_CUSTOM;
			const bool hasModel = modelRenderer->GetModel();
			const auto boundingSphere = hasCustomBoundingSphere ? &modelRenderer->GetCustomBoundingSphere() : hasModel ? &modelRenderer->GetModel()->GetBoundingSphere() : nullptr;
			const auto& actorPosition = p_actor.GetComponent<Transform>()->GetWorldPosition();
			const auto& actorScale = p_actor.GetComponent<Transform>()->GetWorldScale();
			const auto scaleFactor = std::max(std::max(actorScale.x, actorScale.y), actorScale.z);

			distance = std::max(distance, boundingSphere ? (boundingSphere->radius + Vector3::Length(boundingSphere->position)) * scaleFactor * 2.0f : 10.0f);
		}

		for (auto child : p_actor.GetChildren())
			distance = std::max(distance, GetActorFocusDist(*child));
	}*/

	return distance;
}

void LitchiEditor::CameraController::HandleInputs(float p_deltaTime, bool p_isFocus, bool p_isHovered)
{
	if (!m_cameraDestinations.empty())
	{
		//m_currentMovementSpeed = LitchiRuntime::Math::Zero;

		while (m_cameraDestinations.size() != 1)
			m_cameraDestinations.pop();

		auto& [destPos, destRotation] = m_cameraDestinations.front();

		float t = m_focusLerpCoefficient * p_deltaTime;

		if (Vector3::Distance(m_camera->GetPosition(), destPos) <= 0.03f)
		{
			m_camera->SetPosition(destPos);
			m_camera->SetRotation(destRotation);
			m_cameraDestinations.pop();
		}
		else
		{
			m_camera->SetPosition(Vector3::Lerp(m_camera->GetPosition(), destPos, t));
			m_camera->SetRotation(Quaternion::Lerp(m_camera->GetRotation(), destRotation, t));
		}
		return;
	}

	if(!p_isFocus)
	{
		return;
	}

	static const float movement_speed_max = 5.0f;
	static float movement_acceleration = 1.0f;
	static const float movement_drag = 10.0f;
	static const float wheel_speed = 5.0f;
	Vector3 movement_direction = Vector3::Zero;
	float delta_time = static_cast<float>(p_deltaTime);

	// Detect if fps control should be activated
	{
		// Initiate control only when the mouse is within the viewport
		m_is_controlled_by_keyboard_mouse = false;
		/*if ((InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_RIGHT) == EMouseButtonState::MOUSE_DOWN ||
			InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_MIDDLE) == EMouseButtonState::MOUSE_DOWN) &&
			p_isHovered)*/
		if ((InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_RIGHT) == EMouseButtonState::MOUSE_DOWN ||
			InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_MIDDLE) == EMouseButtonState::MOUSE_DOWN))
		{
			m_is_controlled_by_keyboard_mouse = true;
		}
	}

	// Cursor visibility and position
	{
		// Toggle mouse cursor and adjust mouse position
		if (m_is_controlled_by_keyboard_mouse && !m_fps_control_cursor_hidden)
		{
			m_mouse_last_position = InputManager::GetMousePosition();

			//if(InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_MIDDLE) == EMouseButtonState::MOUSE_DOWN)
			//{
			//	InputManager::SetMouseCursorVisible(true);
			//	InputManager::Set
			//	m_fps_control_cursor_hidden = false;
			//}else
			{
				InputManager::SetMouseCursorVisible(false);
				m_fps_control_cursor_hidden = true;
			}
		}
		else if (!m_is_controlled_by_keyboard_mouse && m_fps_control_cursor_hidden)
		{
			InputManager::SetMousePosition(m_mouse_last_position);
			// todo:
			InputManager::SetMouseCursorVisible(true);
			

			m_fps_control_cursor_hidden = false;
		}
	}

	m_movement_scroll_accumulator = 0;
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
			m_first_person_rotation.x = GetRotation().Yaw();
			m_first_person_rotation.y = GetRotation().Pitch();

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
			m_camera->SetRotation(rotation);
		}

		// Keyboard movement direction
		{
			// Compute direction
			if (InputManager::GetKeyState(EKey::KEY_W) == EKeyState::KEY_DOWN) movement_direction += m_camera->GetForward();
			if (InputManager::GetKeyState(EKey::KEY_S) == EKeyState::KEY_DOWN) movement_direction += m_camera->GetBackward();
			if (InputManager::GetKeyState(EKey::KEY_D) == EKeyState::KEY_DOWN) movement_direction += m_camera->GetRight();
			if (InputManager::GetKeyState(EKey::KEY_A) == EKeyState::KEY_DOWN) movement_direction += m_camera->GetLeft();
			if (InputManager::GetKeyState(EKey::KEY_Q) == EKeyState::KEY_DOWN) movement_direction += Vector3::Left;
			if (InputManager::GetKeyState(EKey::KEY_E) == EKeyState::KEY_DOWN) movement_direction += Vector3::Right;
			if (InputManager::GetMouseButtonState(EMouseButton::MOUSE_BUTTON_MIDDLE) == EMouseButtonState::MOUSE_DOWN)
			{
				auto mouseMoveDir = Vector3(InputManager::GetMouseDelta().x, -InputManager::GetMouseDelta().y, 0.0f);
				mouseMoveDir.Normalize();
				movement_direction += mouseMoveDir;
			}


			movement_direction.Normalize();
		}

		// Wheel delta (used to adjust movement speed)
		{
			// Accumulate
			m_movement_scroll_accumulator += InputManager::GetMouseWheelDelta().y * wheel_speed;
		}
	}

	// Translation
	{
		Vector3 translation = (movement_acceleration * movement_direction + m_movement_scroll_accumulator * (m_camera->GetForward()));

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
			m_camera->SetPosition(m_camera->GetPosition()+m_movement_speed);
		}
	}

}

void LitchiEditor::CameraController::SetPosition(const Vector3 & p_position)
{
	m_camera->SetPosition(p_position);
}

void LitchiEditor::CameraController::SetRotation(const Quaternion & p_rotation)
{
	m_camera->SetRotation(p_rotation);
}

void LitchiEditor::CameraController::MoveToTarget(GameObject* target)
{
	auto goWorldPos = target->GetComponent<Transform>()->GetPosition();
	m_cameraDestinations.push({ goWorldPos - m_camera->GetRotation() * LitchiRuntime::Vector3::Forward * GetActorFocusDist(target), m_camera->GetRotation() });
}

const Vector3& LitchiEditor::CameraController::GetPosition() const
{
	return m_camera->GetPosition();
}

const Quaternion& LitchiEditor::CameraController::GetRotation() const
{
	return m_camera->GetRotation();
}

Vector3 RemoveRoll(const Vector3& p_ypr)
{
	Vector3 result = p_ypr;

	if (result.z >= 179.0f || result.z <= -179.0f)
	{
		result.y += result.z;
		result.x = 180.0f - result.x;
		result.z = 0.0f;
	}

	if (result.y > 180.0f) result.y -= 360.0f;
	if (result.y < -180.0f) result.y += 360.0f;

	return result;
}

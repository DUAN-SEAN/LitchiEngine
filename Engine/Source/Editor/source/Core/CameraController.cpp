
#include <algorithm>

#include "Editor/include/Core/CameraController.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

#include "Runtime/Core/Math/MathHelper.h"

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
	m_camera(p_camera),
	m_cameraPosition(p_position),
	m_cameraRotation(p_rotation),
	m_enableFocusInputs(p_enableFocusInputs)
{
	// 设置相机默认的位置和姿态
	m_cameraPosition = Vector3(10.0f, 10.0f, 10.0f);
	
	m_cameraRotation = Quaternion::FromEulerAngles((Vector3(Math::Helper::DegreesToRadians(-45.0f), Math::Helper::DegreesToRadians(45.0f), 0.0f)));

	m_camera->SetFovHorizontalDeg(60.0f);
	m_camera->SetPosition(m_cameraPosition);
	m_camera->SetRotation(m_cameraRotation);
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

void LitchiEditor::CameraController::HandleInputs(float p_deltaTime)
{
	auto selectGO = ApplicationEditor::Instance()->GetSelectGameObject();

	if(InputManager::IsKeyPressed(EKey::KEY_R))
	{
		m_cameraPosition = Vector3(10.0f, 10.0f, 10.0f);
		m_cameraRotation = Quaternion::FromEulerAngles((Vector3(Math::Helper::DegreesToRadians(-45.0f), Math::Helper::DegreesToRadians(45.0f), 0.0f)));
	}

	if (m_view.IsHovered())
	{
		UpdateMouseState();

		//ImGui::GetIO().DisableMouseUpdate = m_rightMousePressed || m_middleMousePressed;

		if (!ImGui::IsAnyItemActive() && m_enableFocusInputs)
		{
			if (selectGO != nullptr)
			{
				
				auto targetPos = selectGO->GetComponent<Transform>()->GetPosition();

				float dist = GetActorFocusDist(selectGO);

				if (InputManager::IsKeyPressed(EKey::KEY_F))
				{
					MoveToTarget(selectGO);
				}

				auto focusObjectFromAngle = [this, &targetPos, &dist]( const Vector3& offset)
				{
					auto targetPosGlm = Vector3(targetPos.x, targetPos.y, targetPos.z);
					auto camPos = targetPosGlm + offset * dist;
					auto direction = (targetPosGlm - camPos).Normalized();
					m_cameraRotation = Quaternion::FromLookRotation(direction, abs(direction.y) == 1.0f ? Vector3::Right : Vector3::Up);
					m_cameraDestinations.push({ camPos, m_cameraRotation });
				};

				if (InputManager::IsKeyPressed(EKey::KEY_UP))			focusObjectFromAngle(Vector3::Up);
				if (InputManager::IsKeyPressed(EKey::KEY_DOWN))		focusObjectFromAngle(-Vector3::Up);
				if (InputManager::IsKeyPressed(EKey::KEY_RIGHT))		focusObjectFromAngle(Vector3::Right);
				if (InputManager::IsKeyPressed(EKey::KEY_LEFT))		focusObjectFromAngle(-Vector3::Right);
				if (InputManager::IsKeyPressed(EKey::KEY_PAGE_UP))	focusObjectFromAngle(Vector3::Forward);
				if (InputManager::IsKeyPressed(EKey::KEY_PAGE_DOWN))	focusObjectFromAngle(-Vector3::Forward);
			}
		}
	}

	if (!m_cameraDestinations.empty())
	{
		m_currentMovementSpeed = Vector3::Zero;

		while (m_cameraDestinations.size() != 1)
			m_cameraDestinations.pop();

		auto& [destPos, destRotation] = m_cameraDestinations.front();

		float t = m_focusLerpCoefficient * p_deltaTime;

		if (Vector3::Distance(m_cameraPosition, destPos) <= 0.03f)
		{
			m_cameraPosition = destPos;
			m_cameraRotation = destRotation;
			m_cameraDestinations.pop();
		}
		else
		{
			m_cameraPosition = Vector3::Lerp(m_cameraPosition, destPos, t);
			m_cameraRotation = Quaternion::Lerp(m_cameraRotation, destRotation, t);
		}
	} 
	else
	{
		if (m_rightMousePressed || m_middleMousePressed || m_leftMousePressed)
		{
			auto [xPos, yPos] = InputManager::GetMousePosition();

			bool wasFirstMouse = m_firstMouse;

			if (m_firstMouse)
			{
				m_lastMousePosX = xPos;
				m_lastMousePosY = yPos;
				m_firstMouse = false;
			}

			Vector2 mouseOffset
			{
				static_cast<float>(xPos - m_lastMousePosX),
				static_cast<float>(m_lastMousePosY - yPos)
			};

			m_lastMousePosX = xPos;
			m_lastMousePosY = yPos;

			if (m_rightMousePressed)
			{
				HandleCameraFPSMouse(mouseOffset, wasFirstMouse);
			}
			else
			{
				if (m_middleMousePressed)
				{
					if (InputManager::GetKeyState(EKey::KEY_LEFT_ALT) == EKeyState::KEY_DOWN)
					{
						if (selectGO != nullptr)
						{
							HandleCameraOrbit(mouseOffset, wasFirstMouse);
						}
					}
					else
					{
						HandleCameraPanning(mouseOffset, wasFirstMouse);
					}
				}
			}
		}

		if (m_view.IsHovered())
		{
			HandleCameraZoom();
		}

		HandleCameraFPSKeyboard(p_deltaTime);
	}

	// DEBUG_LOG_INFO("HandleCameraFPSMouse xAng:{},yAng:{},zAng:{}", m_xyz.x, m_xyz.y, m_xyz.z);
	// DEBUG_LOG_INFO("HandleCameraFPSMouse x:{},y:{},z:{}", m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z);
}

void LitchiEditor::CameraController::MoveToTarget(GameObject* p_target)
{
	auto goWorldPos = p_target->GetComponent<Transform>()->GetPosition();

	auto targetPosGlm = Vector3(goWorldPos.x, goWorldPos.y, goWorldPos.z);
	m_cameraDestinations.push({ targetPosGlm - m_cameraRotation * Vector3::Forward * GetActorFocusDist(p_target), m_cameraRotation });
}

void LitchiEditor::CameraController::SetSpeed(float p_speed)
{
	m_cameraMoveSpeed = p_speed;
}

float LitchiEditor::CameraController::GetSpeed() const
{
	return m_cameraMoveSpeed;
}

void LitchiEditor::CameraController::SetPosition(const Vector3 & p_position)
{
	m_cameraPosition = p_position;
}

void LitchiEditor::CameraController::SetRotation(const Quaternion & p_rotation)
{
	m_cameraRotation = p_rotation;
}

const Vector3& LitchiEditor::CameraController::GetPosition() const
{
	return m_cameraPosition;
}

const Quaternion& LitchiEditor::CameraController::GetRotation() const
{
	return m_cameraRotation;
}

bool LitchiEditor::CameraController::IsRightMousePressed() const
{
	return m_rightMousePressed;
}

void LitchiEditor::CameraController::HandleCameraPanning(const Vector2& p_mouseOffset, bool p_firstMouset)
{
	m_window.SetCursorShape(ECursorShape::HAND);

	auto mouseOffset = p_mouseOffset * m_cameraDragSpeed;

	m_cameraPosition += m_cameraRotation * Vector3::Right * mouseOffset.x;
	m_cameraPosition -= m_cameraRotation * Vector3::Up * mouseOffset.y;
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

void LitchiEditor::CameraController::HandleCameraOrbit(const Vector2& p_mouseOffset, bool p_firstMouse)
{
	/*auto selectGO = ApplicationEditor::Instance()->GetSelectGameObject();
	auto mouseOffset = p_mouseOffset * m_cameraOrbitSpeed;

	if (p_firstMouse)
	{
		m_xyz = glm::eulerAngles(m_cameraRotation);
		m_xyz = RemoveRoll(m_xyz);
		m_orbitTarget = selectGO->GetComponent<Transform>();
		m_orbitStartOffset = -LitchiRuntime::Math::Forward * glm::distance(m_orbitTarget->position(), m_cameraPosition);
	}

	m_xyz.y -= mouseOffset.x;
	m_xyz.x += -mouseOffset.y;
	m_xyz.y = std::max(std::min(m_xyz.y, 90.0f), -90.0f);
	m_xyz.x = std::max(std::min(m_xyz.x, 90.0f), -90.0f);*/

	// TODO 绑定相机transform 和 物体Transform 目前还不能实现
	/*auto target = selectGO->GetComponent<Transform>();
	OvMaths::FTransform pivotTransform(target.GetWorldPosition());
	OvMaths::FTransform cameraTransform(m_orbitStartOffset);
	cameraTransform.SetParent(pivotTransform);
	pivotTransform.RotateLocal(Quaternion(m_xyz));
	m_cameraPosition = cameraTransform.GetWorldPosition();
	m_cameraRotation = cameraTransform.GetWorldRotation();*/
}

void LitchiEditor::CameraController::HandleCameraZoom()
{
	m_cameraPosition += m_cameraRotation * Vector3::Forward * ImGui::GetIO().MouseWheel;
}

void LitchiEditor::CameraController::HandleCameraFPSMouse(const Vector2& p_mouseOffset, bool p_firstMouse)
{
	//DEBUG_LOG_INFO("HandleCameraFPSMouse xAng:{},yAng:{},zAng:{}", m_xyz.x, m_xyz.y, m_xyz.z);

	auto mouseOffset = p_mouseOffset * m_mouseSensitivity;

	if (p_firstMouse)
	{
		auto& eulerAngles = m_cameraRotation.ToEulerAngles();

		// m_xyz =   glm::degrees(glm::eulerAngles(m_cameraRotation));
		m_xyz = eulerAngles;
		m_xyz = RemoveRoll(m_xyz);
	}

	// 万向节(x,y,z)(pitch, yaw, roll)
	//-180 <Yaw<= 180  -90<= Pitch<= 90  -180 <Roll<= 180 if (Pitch == -90 || Pitch == 90) Roll = 0
	m_xyz.y -= mouseOffset.x;
	m_xyz.x += mouseOffset.y;
	m_xyz.y = std::max(std::min(m_xyz.y, 90.0f), -90.0f);
	m_xyz.x = std::max(std::min(m_xyz.x, 90.0f), -90.0f);

	// x , y z (pitch, yaw, roll)
	m_cameraRotation = Quaternion::FromEulerAngles(Vector3(Math::Helper::DegreesToRadians(m_xyz.x), Math::Helper::DegreesToRadians(m_xyz.y), Math::Helper::DegreesToRadians(0.0)));
}

void LitchiEditor::CameraController::HandleCameraFPSKeyboard(float p_deltaTime)
{
	m_targetSpeed = Vector3(0.f, 0.f, 0.f);

	if (m_rightMousePressed)
	{
		bool run = InputManager::GetKeyState(EKey::KEY_LEFT_SHIFT) == EKeyState::KEY_DOWN;
		float velocity = m_cameraMoveSpeed * p_deltaTime * (run ? 2.0f : 1.0f);

		if (InputManager::GetKeyState(EKey::KEY_W) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * Vector3::Forward * velocity;
		if (InputManager::GetKeyState(EKey::KEY_S) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * Vector3::Forward * -velocity;
		if (InputManager::GetKeyState(EKey::KEY_A) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * Vector3::Right * -velocity;
		if (InputManager::GetKeyState(EKey::KEY_D) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * Vector3::Right * velocity;
		if (InputManager::GetKeyState(EKey::KEY_E) == EKeyState::KEY_DOWN)
			m_targetSpeed += Vector3{0.0f, -velocity, 0.0f};
		if (InputManager::GetKeyState(EKey::KEY_Q) == EKeyState::KEY_DOWN)
			m_targetSpeed += Vector3{0.0f, velocity, 0.0f};
	}

	m_currentMovementSpeed = Vector3::Lerp(m_currentMovementSpeed, m_targetSpeed, 10.0f * p_deltaTime);
	m_cameraPosition += m_currentMovementSpeed;
}

void LitchiEditor::CameraController::UpdateMouseState()
{
	if (InputManager::IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_LEFT))
		m_leftMousePressed = true;

	if (InputManager::IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_LEFT))
	{
		m_leftMousePressed = false;
		m_firstMouse = true;
	}

	if (InputManager::IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_MIDDLE))
		m_middleMousePressed = true;

	if (InputManager::IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_MIDDLE))
	{
		m_middleMousePressed = false;
		m_firstMouse = true;
	}

	if (InputManager::IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = true;
		m_window.SetCursorMode(ECursorMode::DISABLED);
	}

	if (InputManager::IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = false;
		m_firstMouse = true;
		m_window.SetCursorMode(ECursorMode::NORMAL);
	}
}

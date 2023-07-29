
#include <algorithm>

#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/compatibility.hpp"

#include "Editor/include/Core/CameraController.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/render_camera.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

#include "Runtime/Core/Math/MathHelper.h"

LitchiEditor::CameraController::CameraController
(
	AView& p_view,
	RenderCamera* p_camera,
	glm::vec3& p_position,
	glm::quat& p_rotation,
	bool p_enableFocusInputs
) :
	m_inputManager(*ApplicationEditor::Instance()->inputManager),
	m_window(*ApplicationEditor::Instance()->window),
	m_view(p_view),
	m_camera(p_camera),
	m_cameraPosition(p_position),
	m_cameraRotation(p_rotation),
	m_enableFocusInputs(p_enableFocusInputs)
{
	// 设置相机默认的位置和姿态
	m_cameraPosition = glm::vec3(10.0f, 10.0f, 10.0f);
	m_cameraRotation = glm::quat(glm::radians(glm::vec3(-45.0f, 45.0f, 0.0f)));

	m_camera->SetFov(60.0f);
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

			distance = std::max(distance, boundingSphere ? (boundingSphere->radius + glm::vec3::Length(boundingSphere->position)) * scaleFactor * 2.0f : 10.0f);
		}

		for (auto child : p_actor.GetChildren())
			distance = std::max(distance, GetActorFocusDist(*child));
	}*/

	return distance;
}

void LitchiEditor::CameraController::HandleInputs(float p_deltaTime)
{
	auto selectGO = ApplicationEditor::Instance()->GetSelectGameObject();

	if(m_inputManager.IsKeyPressed(EKey::KEY_R))
	{
		m_cameraPosition = glm::vec3(10.0f, 10.0f, 10.0f);
		m_cameraRotation = glm::quat(glm::radians(glm::vec3(-45.0f, 45.0f, 0.0f)));
	}

	if (m_view.IsHovered())
	{
		UpdateMouseState();

		ImGui::GetIO().DisableMouseUpdate = m_rightMousePressed || m_middleMousePressed;

		if (!ImGui::IsAnyItemActive() && m_enableFocusInputs)
		{
			if (selectGO != nullptr)
			{
				
				auto targetPos = selectGO->GetComponent<Transform>()->GetWorldPosition();

				float dist = GetActorFocusDist(selectGO);

				if (m_inputManager.IsKeyPressed(EKey::KEY_F))
				{
					MoveToTarget(selectGO);
				}

				auto focusObjectFromAngle = [this, &targetPos, &dist]( const glm::vec3& offset)
				{
					auto camPos = targetPos + offset * dist;
					auto direction = glm::normalize(targetPos - camPos);
					m_cameraRotation = LitchiRuntime::Math::LookAt(direction, abs(direction.y) == 1.0f ? LitchiRuntime::Math::Right : LitchiRuntime::Math::Up);
					m_cameraDestinations.push({ camPos, m_cameraRotation });
				};

				if (m_inputManager.IsKeyPressed(EKey::KEY_UP))			focusObjectFromAngle(LitchiRuntime::Math::Up);
				if (m_inputManager.IsKeyPressed(EKey::KEY_DOWN))		focusObjectFromAngle(-LitchiRuntime::Math::Up);
				if (m_inputManager.IsKeyPressed(EKey::KEY_RIGHT))		focusObjectFromAngle(LitchiRuntime::Math::Right);
				if (m_inputManager.IsKeyPressed(EKey::KEY_LEFT))		focusObjectFromAngle(-LitchiRuntime::Math::Right);
				if (m_inputManager.IsKeyPressed(EKey::KEY_PAGE_UP))	focusObjectFromAngle(LitchiRuntime::Math::Forward);
				if (m_inputManager.IsKeyPressed(EKey::KEY_PAGE_DOWN))	focusObjectFromAngle(-LitchiRuntime::Math::Forward);
			}
		}
	}

	if (!m_cameraDestinations.empty())
	{
		m_currentMovementSpeed = LitchiRuntime::Math::Zero;

		while (m_cameraDestinations.size() != 1)
			m_cameraDestinations.pop();

		auto& [destPos, destRotation] = m_cameraDestinations.front();

		float t = m_focusLerpCoefficient * p_deltaTime;

		if (glm::distance(m_cameraPosition, destPos) <= 0.03f)
		{
			m_cameraPosition = destPos;
			m_cameraRotation = destRotation;
			m_cameraDestinations.pop();
		}
		else
		{
			m_cameraPosition = glm::lerp(m_cameraPosition, destPos, t);
			m_cameraRotation = LitchiRuntime::Math::Lerp(m_cameraRotation, destRotation, t);
		}
	} 
	else
	{
		if (m_rightMousePressed || m_middleMousePressed || m_leftMousePressed)
		{
			auto [xPos, yPos] = m_inputManager.GetMousePosition();

			bool wasFirstMouse = m_firstMouse;

			if (m_firstMouse)
			{
				m_lastMousePosX = xPos;
				m_lastMousePosY = yPos;
				m_firstMouse = false;
			}

			glm::vec2 mouseOffset
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
					if (m_inputManager.GetKeyState(EKey::KEY_LEFT_ALT) == EKeyState::KEY_DOWN)
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
	auto goWorldPos = p_target->GetComponent<Transform>()->GetTransform().GetWorldPosition();
	m_cameraDestinations.push({ goWorldPos - m_cameraRotation * LitchiRuntime::Math::Forward * GetActorFocusDist(p_target), m_cameraRotation });
}

void LitchiEditor::CameraController::SetSpeed(float p_speed)
{
	m_cameraMoveSpeed = p_speed;
}

float LitchiEditor::CameraController::GetSpeed() const
{
	return m_cameraMoveSpeed;
}

void LitchiEditor::CameraController::SetPosition(const glm::vec3 & p_position)
{
	m_cameraPosition = p_position;
}

void LitchiEditor::CameraController::SetRotation(const glm::quat & p_rotation)
{
	m_cameraRotation = p_rotation;
}

const glm::vec3& LitchiEditor::CameraController::GetPosition() const
{
	return m_cameraPosition;
}

const glm::quat& LitchiEditor::CameraController::GetRotation() const
{
	return m_cameraRotation;
}

bool LitchiEditor::CameraController::IsRightMousePressed() const
{
	return m_rightMousePressed;
}

void LitchiEditor::CameraController::HandleCameraPanning(const glm::vec2& p_mouseOffset, bool p_firstMouset)
{
	m_window.SetCursorShape(ECursorShape::HAND);

	auto mouseOffset = p_mouseOffset * m_cameraDragSpeed;

	m_cameraPosition += m_cameraRotation * LitchiRuntime::Math::Right * mouseOffset.x;
	m_cameraPosition -= m_cameraRotation * LitchiRuntime::Math::Up * mouseOffset.y;
}

glm::vec3 RemoveRoll(const glm::vec3& p_ypr)
{
	glm::vec3 result = p_ypr;

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

void LitchiEditor::CameraController::HandleCameraOrbit(const glm::vec2& p_mouseOffset, bool p_firstMouse)
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
	pivotTransform.RotateLocal(glm::quat(m_xyz));
	m_cameraPosition = cameraTransform.GetWorldPosition();
	m_cameraRotation = cameraTransform.GetWorldRotation();*/
}

void LitchiEditor::CameraController::HandleCameraZoom()
{
	m_cameraPosition += m_cameraRotation * LitchiRuntime::Math::Forward * ImGui::GetIO().MouseWheel;
}

void LitchiEditor::CameraController::HandleCameraFPSMouse(const glm::vec2& p_mouseOffset, bool p_firstMouse)
{
	//DEBUG_LOG_INFO("HandleCameraFPSMouse xAng:{},yAng:{},zAng:{}", m_xyz.x, m_xyz.y, m_xyz.z);

	auto mouseOffset = p_mouseOffset * m_mouseSensitivity;

	if (p_firstMouse)
	{
		m_xyz = glm::degrees(glm::eulerAngles(m_cameraRotation));
		m_xyz = RemoveRoll(m_xyz);
	}

	// 万向节(x,y,z)(pitch, yaw, roll)
	//-180 <Yaw<= 180  -90<= Pitch<= 90  -180 <Roll<= 180 if (Pitch == -90 || Pitch == 90) Roll = 0
	m_xyz.y -= mouseOffset.x;
	m_xyz.x += mouseOffset.y;
	m_xyz.y = std::max(std::min(m_xyz.y, 90.0f), -90.0f);
	m_xyz.x = std::max(std::min(m_xyz.x, 90.0f), -90.0f);

	// x , y z (pitch, yaw, roll)
	m_cameraRotation = glm::quat(glm::vec3(glm::radians(m_xyz.x), glm::radians(m_xyz.y), glm::radians(0.0)));
}

void LitchiEditor::CameraController::HandleCameraFPSKeyboard(float p_deltaTime)
{
	m_targetSpeed = glm::vec3(0.f, 0.f, 0.f);

	if (m_rightMousePressed)
	{
		bool run = m_inputManager.GetKeyState(EKey::KEY_LEFT_SHIFT) == EKeyState::KEY_DOWN;
		float velocity = m_cameraMoveSpeed * p_deltaTime * (run ? 2.0f : 1.0f);

		if (m_inputManager.GetKeyState(EKey::KEY_W) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * LitchiRuntime::Math::Forward * velocity;
		if (m_inputManager.GetKeyState(EKey::KEY_S) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * LitchiRuntime::Math::Forward * -velocity;
		if (m_inputManager.GetKeyState(EKey::KEY_A) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * LitchiRuntime::Math::Right * -velocity;
		if (m_inputManager.GetKeyState(EKey::KEY_D) == EKeyState::KEY_DOWN)
			m_targetSpeed += m_cameraRotation * LitchiRuntime::Math::Right * velocity;
		if (m_inputManager.GetKeyState(EKey::KEY_E) == EKeyState::KEY_DOWN)
			m_targetSpeed += glm::vec3{0.0f, -velocity, 0.0f};
		if (m_inputManager.GetKeyState(EKey::KEY_Q) == EKeyState::KEY_DOWN)
			m_targetSpeed += glm::vec3{0.0f, velocity, 0.0f};
	}

	m_currentMovementSpeed = glm::lerp(m_currentMovementSpeed, m_targetSpeed, 10.0f * p_deltaTime);
	m_cameraPosition += m_currentMovementSpeed;
}

void LitchiEditor::CameraController::UpdateMouseState()
{
	if (m_inputManager.IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_LEFT))
		m_leftMousePressed = true;

	if (m_inputManager.IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_LEFT))
	{
		m_leftMousePressed = false;
		m_firstMouse = true;
	}

	if (m_inputManager.IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_MIDDLE))
		m_middleMousePressed = true;

	if (m_inputManager.IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_MIDDLE))
	{
		m_middleMousePressed = false;
		m_firstMouse = true;
	}

	if (m_inputManager.IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = true;
		m_window.SetCursorMode(ECursorMode::DISABLED);
	}

	if (m_inputManager.IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = false;
		m_firstMouse = true;
		m_window.SetCursorMode(ECursorMode::NORMAL);
	}
}

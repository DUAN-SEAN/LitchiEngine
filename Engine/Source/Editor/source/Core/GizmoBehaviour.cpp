

#include "Editor/include/Core/GizmoBehaviour.h"

#include "Runtime/Core/Window/Inputs/EKey.h"
#include "Runtime/Core/Window/Inputs/EKeyState.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

float SnapValue(float p_value, float p_step)
{
	return p_value - std::fmod(p_value, p_step);
}

bool LitchiEditor::GizmoBehaviour::IsSnappedBehaviourEnabled() const
{
	/*using namespace OvWindowing::Inputs;

	const auto& inputManager = EDITOR_CONTEXT(inputManager);
	return inputManager->GetKeyState(LitchiRuntime::EKey::KEY_LEFT_CONTROL) == LitchiRuntime::EKeyState::KEY_DOWN || inputManager->GetKeyState(EKey::KEY_RIGHT_CONTROL) == EKeyState::KEY_DOWN;*/
	return false;
}

void LitchiEditor::GizmoBehaviour::StartPicking(LitchiRuntime::GameObject* p_target, const glm::vec3& p_cameraPosition, EGizmoOperation p_operation, EDirection p_direction)
{
	m_target = p_target;
	m_firstMouse = true;
	m_originalTransform = m_target->GetComponent<LitchiRuntime::Transform>();
	//m_originalTransform = p_target.transform.GetFTransform();

	m_distanceToActor = glm::distance(p_cameraPosition, m_originalTransform->GetWorldPosition());
	//m_distanceToActor = glm::distance(p_cameraPosition, m_target->transform.GetWorldPosition());
	m_currentOperation = p_operation;
	m_direction = p_direction;
}

void LitchiEditor::GizmoBehaviour::StopPicking()
{
	m_target = nullptr;
}

glm::vec3 LitchiEditor::GizmoBehaviour::GetFakeDirection() const
{
	auto result = glm::vec3();

	/*switch (m_direction)
	{
	case LitchiEditor::GizmoBehaviour::EDirection::X:
		
		result = glm::vec3::Right;
		break;
	case LitchiEditor::GizmoBehaviour::EDirection::Y:
		result = glm::vec3::Up;
		break;
	case LitchiEditor::GizmoBehaviour::EDirection::Z:
		result = glm::vec3::Forward;
		break;
	}*/

	return result;
}

glm::vec3 LitchiEditor::GizmoBehaviour::GetRealDirection(bool p_relative) const
{
	auto result = glm::vec3();

	/*switch (m_direction)
	{
	case LitchiEditor::GizmoBehaviour::EDirection::X:
		result = p_relative ? m_originalTransform.GetWorldRight() : m_originalTransform.GetLocalRight();
		break;
	case LitchiEditor::GizmoBehaviour::EDirection::Y:
		result = p_relative ? m_originalTransform.GetWorldUp() : m_originalTransform.GetLocalUp();
		break;
	case LitchiEditor::GizmoBehaviour::EDirection::Z:
		result = p_relative ? m_originalTransform.GetWorldForward() : m_originalTransform.GetLocalForward();
		break;
	}*/

	return result;
}

glm::vec2 LitchiEditor::GizmoBehaviour::GetScreenDirection(const glm::mat4& p_viewMatrix, const glm::mat4& p_projectionMatrix, const glm::vec2& p_viewSize) const
{
	//auto start = m_originalTransform.GetWorldPosition();
	//auto end = m_originalTransform.GetWorldPosition() + GetRealDirection(true) * 0.01f;

	//auto start2D = glm::vec2();
	//{
	//	auto clipSpacePos = p_projectionMatrix * (p_viewMatrix * OvMaths::glm::vec4{ start.x, start.y, start.z, 1.0f });
	//	auto ndcSpacePos = glm::vec3{ clipSpacePos.x, clipSpacePos.y, clipSpacePos.z } / clipSpacePos.w;
	//	auto windowSpacePos = ((glm::vec2{ ndcSpacePos.x, ndcSpacePos.y } + 1.0f) / 2.0f);
	//	windowSpacePos.x *= p_viewSize.x;
	//	windowSpacePos.y *= p_viewSize.y;
	//	start2D = windowSpacePos;
	//}

	//auto end2D = glm::vec2();
	//{
	//	auto clipSpacePos = p_projectionMatrix * (p_viewMatrix * OvMaths::glm::vec4{ end.x, end.y, end.z, 1.0f });
	//	auto ndcSpacePos = glm::vec3{ clipSpacePos.x, clipSpacePos.y, clipSpacePos.z } / clipSpacePos.w;
	//	auto windowSpacePos = ((glm::vec2{ ndcSpacePos.x, ndcSpacePos.y } + 1.0f) / 2.0f);
	//	windowSpacePos.x *= p_viewSize.x;
	//	windowSpacePos.y *= p_viewSize.y;
	//	end2D = windowSpacePos;
	//}

	//auto result = end2D - start2D;

	//result.y *= -1; // Screen coordinates are reversed, so we inverse the Y

	//return glm::vec2::Normalize(result);
	return glm::vec2();
}

void LitchiEditor::GizmoBehaviour::ApplyTranslation(const glm::mat4& p_viewMatrix, const glm::mat4& p_projectionMatrix, const glm::vec2& p_viewSize) const
{
	/*auto unitsPerPixel = 0.001f * m_distanceToActor;
	auto originPosition = m_originalTransform.GetLocalPosition();

	auto screenDirection = GetScreenDirection(p_viewMatrix, p_projectionMatrix, p_viewSize);

	auto totalDisplacement = m_currentMouse - m_originMouse;
	auto translationCoefficient = glm::vec2::Dot(totalDisplacement, screenDirection) * unitsPerPixel;

	if (IsSnappedBehaviourEnabled())
	{
		translationCoefficient = SnapValue(translationCoefficient, OvEditor::Settings::EditorSettings::TranslationSnapUnit);
	}

	m_target->transform.SetLocalPosition(originPosition + GetRealDirection() * translationCoefficient);*/
}

void LitchiEditor::GizmoBehaviour::ApplyRotation(const glm::mat4& p_viewMatrix, const glm::mat4& p_projectionMatrix, const glm::vec2& p_viewSize) const
{
	/*auto unitsPerPixel = 0.2f;
	auto originRotation = m_originalTransform.GetLocalRotation();

	auto screenDirection = GetScreenDirection(p_viewMatrix, p_projectionMatrix, p_viewSize);
	screenDirection = glm::vec2(-screenDirection.y, screenDirection.x);

	auto totalDisplacement = m_currentMouse - m_originMouse;
	auto rotationCoefficient = glm::vec2::Dot(totalDisplacement, screenDirection) * unitsPerPixel;

	if (IsSnappedBehaviourEnabled())
	{
		rotationCoefficient = SnapValue(rotationCoefficient, OvEditor::Settings::EditorSettings::RotationSnapUnit);
	}

	auto rotationToApply = glm::quat(glm::vec3(GetFakeDirection() * rotationCoefficient));
	m_target->transform.SetLocalRotation(originRotation * rotationToApply);*/
}

void LitchiEditor::GizmoBehaviour::ApplyScale(const glm::mat4& p_viewMatrix, const glm::mat4& p_projectionMatrix, const glm::vec2& p_viewSize) const
{
	//auto unitsPerPixel = 0.01f;
	//auto originScale = m_originalTransform.GetLocalScale();

	//auto screenDirection = GetScreenDirection(p_viewMatrix, p_projectionMatrix, p_viewSize);

	//auto totalDisplacement = m_currentMouse - m_originMouse;
	//auto scaleCoefficient = glm::vec2::Dot(totalDisplacement, screenDirection) * unitsPerPixel;

	//if (IsSnappedBehaviourEnabled())
	//{
	//	scaleCoefficient = SnapValue(scaleCoefficient, OvEditor::Settings::EditorSettings::ScalingSnapUnit);
	//}

	//auto newScale = originScale + GetFakeDirection() * scaleCoefficient;

	///* Prevent scale from being negative*/
	//newScale.x = std::max(newScale.x, 0.0001f);
	//newScale.y = std::max(newScale.y, 0.0001f);
	//newScale.z = std::max(newScale.z, 0.0001f);

	//m_target->transform.SetLocalScale(newScale);
}

void LitchiEditor::GizmoBehaviour::ApplyOperation(const glm::mat4& p_viewMatrix, const glm::mat4& p_projectionMatrix, const glm::vec2& p_viewSize)
{
	switch (m_currentOperation)
	{
	case EGizmoOperation::TRANSLATE:
		ApplyTranslation(p_viewMatrix, p_projectionMatrix, p_viewSize);
		break;

	case EGizmoOperation::ROTATE:
		ApplyRotation(p_viewMatrix, p_projectionMatrix, p_viewSize);
		break;

	case EGizmoOperation::SCALE:
		ApplyScale(p_viewMatrix, p_projectionMatrix, p_viewSize);
		break;
	}
}

void LitchiEditor::GizmoBehaviour::SetCurrentMouse(const glm::vec2& p_mousePosition)
{
	if (m_firstMouse)
	{
		m_currentMouse = m_originMouse = p_mousePosition;
		m_firstMouse = false;
	}
	else
	{
		m_currentMouse = p_mousePosition;
	}
}

bool LitchiEditor::GizmoBehaviour::IsPicking() const
{
	return m_target;
}

LitchiEditor::GizmoBehaviour::EDirection LitchiEditor::GizmoBehaviour::GetDirection() const
{
	return m_direction;
}
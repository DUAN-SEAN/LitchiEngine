#include "TransformGizmo.h"

#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/UI/ImGui/ImGuizmo/ImGuizmo.h"

using namespace LitchiRuntime;

LitchiRuntime::TransformGizmo::TransformGizmo(Camera* camera) :
	m_camera(camera)
{
}

void TransformGizmo::SetSelectGameObject(GameObject* go)
{
	m_selectObject = go;
}

void LitchiRuntime::TransformGizmo::SetCamera(Camera* camera)
{
	m_camera = camera;
}

void TransformGizmo::_Draw_Impl()
{
	// Enable/disable gizmo
	ImGuizmo::Enable(m_selectObject != nullptr);
	if (!m_selectObject)
		return;

	// Switch between position, rotation and scale operations, with W, E and R respectively
	static ImGuizmo::OPERATION transform_operation = ImGuizmo::TRANSLATE;
	// if (!camera->IsControledInFirstPerson())
	{
		if (LitchiRuntime::InputManager::GetKeyState(LitchiRuntime::EKey::KEY_W) == LitchiRuntime::EKeyState::KEY_DOWN)
		{
			transform_operation = ImGuizmo::TRANSLATE;
		}
		else if (LitchiRuntime::InputManager::GetKeyState(LitchiRuntime::EKey::KEY_E) == LitchiRuntime::EKeyState::KEY_DOWN)
		{
			transform_operation = ImGuizmo::ROTATE;
		}
		else if (LitchiRuntime::InputManager::GetKeyState(LitchiRuntime::EKey::KEY_R) == LitchiRuntime::EKeyState::KEY_DOWN)
		{
			transform_operation = ImGuizmo::SCALE;
		}
	}

	ImGuizmo::MODE transform_space = ImGuizmo::WORLD;

	// Get some data
	const LitchiRuntime::Matrix& matrix_projection = m_camera->GetProjectionMatrix().Transposed();
	const LitchiRuntime::Matrix& matrix_view = m_camera->GetViewMatrix().Transposed();
	auto transform = m_selectObject->GetComponent<LitchiRuntime::Transform>();

	// Begin
	const bool is_orthographic = false;
	ImGuizmo::SetOrthographic(is_orthographic);
	ImGuizmo::BeginFrame();

	// Map transform to ImGuizmo
	LitchiRuntime::Vector3 position = transform->GetPosition();
	LitchiRuntime::Vector3 scale = transform->GetScale();
	LitchiRuntime::Quaternion rotation = transform->GetRotation();

	LitchiRuntime::Matrix transform_matrix = LitchiRuntime::Matrix::GenerateRowFirst(position, rotation, scale);

	// Set viewport rectangle
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

	ImGuizmo::Manipulate(&matrix_view.m00, &matrix_projection.m00, transform_operation, transform_space, &transform_matrix.m00, nullptr, nullptr);

	// Map ImGuizmo to transform
	if (ImGuizmo::IsUsing())
	{
		transform_matrix.Transposed().Decompose(scale, rotation, position);

		transform->SetPosition(position);
		transform->SetRotation(rotation);
		transform->SetScale(scale);
	}
}


#include "Runtime/Core/pch.h"
#include "Editor/include/Panels/SceneView.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Renderer/Light/Light.h"
#include "Runtime/Function/UI/ImGui/ImGui_TransformGizmo.h"

LitchiEditor::SceneView::SceneView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings,
	RendererPath* rendererPath
) : AViewControllable(p_title, p_opened, p_windowSettings, rendererPath,true)
{
	m_camera->SetClearColor({ 0.098f, 0.098f, 0.098f });

	m_transform_gizmo = &CreateWidget<TransformGizmo>(m_camera);
	//m_image->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	//{
	//	std::string path = p_data.first;

	//	switch (PathParser::GetFileType(path))
	//	{
	//	case PathParser::EFileType::SCENE:	EDITOR_EXEC(LoadSceneFromDisk(path));			break;
	//	case PathParser::EFileType::MODEL:	EDITOR_EXEC(CreateActorWithModel(path, true));	break;
	//	}
	//};

	GameObject::DestroyedEvent += std::bind(&SceneView::DeleteActorByInstance, this, std::placeholders::_1);
}

void LitchiEditor::SceneView::UpdateView(float p_deltaTime)
{
	AViewControllable::UpdateView(p_deltaTime);
}

void LitchiEditor::SceneView::_Render_Impl()
{
	PrepareCamera();
}

void LitchiEditor::SceneView::OnDraw()
{
	// mouse picking
	if (ImGui::IsMouseClicked(0) && IsHovered() && ImGui::TransformGizmo::allow_picking())
	{
		if (auto camera = m_camera)
		{
			camera->Pick();
			auto selectedGO= camera->GetSelectedEntity();
			ApplicationEditor::Instance()->SelectActor(selectedGO);// temp
		}
	}

	AView::OnDraw();
}

void LitchiEditor::SceneView::RenderSceneForActorPicking()
{
}

bool IsResizing()
{
	auto cursor = ImGui::GetMouseCursor();

	return
		cursor == ImGuiMouseCursor_ResizeEW ||
		cursor == ImGuiMouseCursor_ResizeNS ||
		cursor == ImGuiMouseCursor_ResizeNWSE ||
		cursor == ImGuiMouseCursor_ResizeNESW ||
		cursor == ImGuiMouseCursor_ResizeAll;;
}

void LitchiEditor::SceneView::HandleActorPicking()
{
}

void LitchiEditor::SceneView::DeleteActorByInstance(GameObject* p_actor)
{
	if (auto camera = m_camera)
	{
		auto selectedGO = camera->GetSelectedEntity();
		if(selectedGO == p_actor)
		{
			camera->SetSelectedEntity(nullptr);
		}
	}
}



#include "Editor/include/Panels/SceneView.h"

#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Renderer/Light/Light.h"

LitchiEditor::SceneView::SceneView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings,
	RHI_Texture* renderTargetTexture
) : AViewControllable(p_title, p_opened, p_windowSettings, renderTargetTexture,true)
{
	m_camera->SetClearColor({ 0.098f, 0.098f, 0.098f });
	m_camera->SetFarPlane(5000.0f);

	m_transform_gizmo = &CreateWidget<TransformGizmo>(nullptr);
	m_renderPath4SceneView = nullptr;
	//m_image->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	//{
	//	std::string path = p_data.first;

	//	switch (PathParser::GetFileType(path))
	//	{
	//	case PathParser::EFileType::SCENE:	EDITOR_EXEC(LoadSceneFromDisk(path));			break;
	//	case PathParser::EFileType::MODEL:	EDITOR_EXEC(CreateActorWithModel(path, true));	break;
	//	}
	//};
}

void LitchiEditor::SceneView::Update(float p_deltaTime)
{
	AViewControllable::Update(p_deltaTime);
}

void LitchiEditor::SceneView::_Render_Impl()
{
	// prepare renderer path 
	if(m_renderPath4SceneView == nullptr)
	{
		m_renderPath4SceneView = new RendererPath(RendererPathType_SceneView);
		m_renderPath4SceneView->m_renderCamera = m_camera;
		m_renderPath4SceneView->m_renderScene = ApplicationBase::Instance()->sceneManager->GetCurrentScene();
		m_renderPath4SceneView->m_depthRenderTarget = nullptr;
		m_renderPath4SceneView->m_colorRenderTarget = nullptr;
	}

	// update renderer path
	Renderer::UpdateRendererPath(RendererPathType_SceneView, m_renderPath4SceneView);
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

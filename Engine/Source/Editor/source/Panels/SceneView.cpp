
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
	m_camera->SetClearColor(Vector3{ 0.098f, 0.098f, 0.098f });
	m_camera->SetFar(5000.0f);

	//m_image->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	//{
	//	std::string path = p_data.first;

	//	switch (PathParser::GetFileType(path))
	//	{
	//	case PathParser::EFileType::SCENE:	EDITOR_EXEC(LoadSceneFromDisk(path));			break;
	//	case PathParser::EFileType::MODEL:	EDITOR_EXEC(CreateActorWithModel(path, true));	break;
	//	}
	//};


	// todo:
	//m_gridMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Grid"));
	//m_gridMaterial.SetBlendable(true);
	//m_gridMaterial.SetBackfaceCulling(false);
	//m_gridMaterial.SetDepthTest(false);

	/*m_shadowMapFbo.Bind();
	m_shadowMapFbo.Resize(2048, 2048);
	m_shadowMapFbo.Unbind();*/


	// 初始化UI相机
	m_camera4UI = new RenderCamera();
	m_camera4UI->set_clear_flag(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);// 不清理颜色, 只清理深度信息
	m_camera4UI->SetProjectionMode(ProjectionMode::ORTHOGRAPHIC);
	m_cameraPosition4UI = Vector3(Vector3(0, 0, -10));
	m_cameraRotation4UI = Quaternion(1,0,0,0);
	auto [winWidth, winHeight] = GetSafeSize();
	m_camera4UI->SetSize(winWidth / 2.0f);
	m_camera4UI->SetNear(-100);
	m_camera4UI->SetFar(100);
	m_camera4UI->CacheMatrices(winWidth, winHeight, m_cameraPosition4UI, m_cameraRotation4UI);
}

void LitchiEditor::SceneView::Update(float p_deltaTime)
{
	/*Scene* scene = ApplicationEditor::Instance()->sceneManager->GetCurrentScene();
	auto go = scene->game_object_vec_.front();
	auto trans = go->GetComponent<Transform>();*/

	/*auto rotation = trans->GetLocalRotation();
	auto next = Quaternion(Vector3(glm::radians(60.0) * p_deltaTime, glm::radians(30.0) * p_deltaTime, 0));
	rotation = rotation * next;
	trans->SetLocalRotation(rotation);*/
	// ApplicationEditor::Instance()->SetSelectGameObject(go);

	AViewControllable::Update(p_deltaTime);

	/*using namespace OvWindowing::Inputs;

	if (IsFocused() && !m_cameraController.IsRightMousePressed())
	{
		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_W))
		{
			m_currentOperation = EGizmoOperation::TRANSLATE;
		}

		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_E))
		{
			m_currentOperation = EGizmoOperation::ROTATE;
		}

		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_R))
		{
			m_currentOperation = EGizmoOperation::SCALE;
		}
	}*/
}

void LitchiEditor::SceneView::_Render_Impl()
{
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

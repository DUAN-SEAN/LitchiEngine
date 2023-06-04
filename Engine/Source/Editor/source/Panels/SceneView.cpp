
#include "Editor/include/Panels/SceneView.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Window/Inputs/EKey.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Renderer/render_camera.h"
#include "Runtime/Function/Renderer/render_system.h"
#include "Runtime/Function/Renderer/Light/Light.h"

LitchiEditor::SceneView::SceneView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings
) : AViewControllable(p_title, p_opened, p_windowSettings, true)
{
	m_camera = new RenderCamera();
	m_camera->SetClearColor(glm::vec3{ 0.098f, 0.098f, 0.098f });
	m_camera->SetFar(5000.0f);

	//m_image->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	//{
	//	std::string path = p_data.first;

	//	switch (OvTools::Utils::PathParser::GetFileType(path))
	//	{
	//	case OvTools::Utils::PathParser::EFileType::SCENE:	EDITOR_EXEC(LoadSceneFromDisk(path));			break;
	//	case OvTools::Utils::PathParser::EFileType::MODEL:	EDITOR_EXEC(CreateActorWithModel(path, true));	break;
	//	}
	//};


	m_gridMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Grid"));
	m_gridMaterial.SetBlendable(true);
	m_gridMaterial.SetBackfaceCulling(false);
	m_gridMaterial.SetDepthTest(false);
}

void LitchiEditor::SceneView::Update(float p_deltaTime)
{
	Scene* scene = SceneManager::GetScene("Default Scene");
	auto go= scene->game_object_vec_.front();
	auto trans = go->GetComponent<Transform>();

	auto rotation = trans->rotation();
	auto next = glm::quat(glm::vec3(glm::radians(60.0) * p_deltaTime, glm::radians(30.0) * p_deltaTime, 0));
	rotation = rotation * next;
	trans->set_rotation(rotation);
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
	// 准备相机数据
	PrepareCamera();

	// 绘制场景
	RenderScene();
}
void LitchiEditor::SceneView::RenderScene()
{
	// 绑定FBO
	m_fbo.Bind();

	// 绑定Light的SSBO
	auto& ssbo= ApplicationEditor::Instance()->lightSSBO;
	ssbo->Bind(0);

	// 测试
	FTransform lightTransform;
	Light light(lightTransform, Light::Type::DIRECTIONAL);
	std::vector<glm::mat4> lightMatrixArr;
	lightMatrixArr.push_back(light.GenerateMatrix());
	ssbo->SendBlocks(lightMatrixArr.data(), sizeof(glm::mat4) * lightMatrixArr.size());

	// 调用管线渲染场景
	// 获取当前需要渲染的相机 和 场景
	RenderCamera* render_camera = m_camera;
	Scene* scene = SceneManager::GetScene("Default Scene");

	render_camera->Clear();
	RenderGrid(m_cameraPosition, glm::vec3(0.098f, 0.898f, 0.098f));

	// 遍历所有的物体,执行MeshRenderer的Render函数
	scene->Foreach([&](GameObject* game_object) {
		if (game_object->active()) {
			game_object->ForeachComponent([&](Component* component) {
				auto* mesh_renderer = dynamic_cast<MeshRenderer*>(component);
				if (mesh_renderer == nullptr) {
					return;
				}
				mesh_renderer->Render(render_camera);
				});
		}
		});

	ssbo->Unbind();

	m_fbo.Unbind();
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

void LitchiEditor::SceneView::RenderGrid(const glm::vec3& p_viewPos, const glm::vec3& p_color)
{
	constexpr float gridSize = 5000.0f;

	// 绘制plane
	glm::mat4 modelMatrix = glm::translate(glm::vec3{ p_viewPos.x, 0.0f, p_viewPos.z }) * glm::scale(glm::vec3{ gridSize * 2.0f, 1.f, gridSize * 2.0f });
	m_gridMaterial.Set("u_Color", p_color);

	// 设置ubo
	/*
		mat4    ubo_Model;
	    mat4    ubo_View;
	    mat4    ubo_Projection;
	    vec3    ubo_ViewPos;
	    float   ubo_Time;
    */
	ApplicationEditor::Instance()->engineUBO->SetSubData(modelMatrix, 0);
	
	uint8_t stateMask = m_gridMaterial.GenerateStateMask();
	LitchiEditor::ApplicationEditor::Instance()->renderer->ApplyStateMask(stateMask);

	/* Draw the mesh */
	m_gridMaterial.Bind(ApplicationEditor::Instance()->editorResources->GetTexture("Empty_Texture"));
	auto planeModel = ApplicationEditor::Instance()->editorResources->GetModel("Plane");
	LitchiEditor::ApplicationEditor::Instance()->renderer->Draw(*planeModel->GetMeshes().front(), EPrimitiveMode::TRIANGLES, m_gridMaterial.GetGPUInstances());
	m_gridMaterial.UnBind();


	LitchiEditor::ApplicationEditor::Instance()->shapeDrawer->DrawLine(glm::vec3(-gridSize + p_viewPos.x, 0.0f, 0.0f), glm::vec3(gridSize + p_viewPos.x, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
	LitchiEditor::ApplicationEditor::Instance()->shapeDrawer->DrawLine(glm::vec3(0.0f, -gridSize + p_viewPos.y, 0.0f), glm::vec3(0.0f, gridSize + p_viewPos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
	LitchiEditor::ApplicationEditor::Instance()->shapeDrawer->DrawLine(glm::vec3(0.0f, 0.0f, -gridSize + p_viewPos.z), glm::vec3(0.0f, 0.0f, gridSize + p_viewPos.z), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
}
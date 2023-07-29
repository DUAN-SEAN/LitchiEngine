
#include "Editor/include/Panels/SceneView.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Window/Inputs/EKey.h"
#include "Runtime/Function/Framework/Component/Light/DirectionalLight.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
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
	m_camera->SetClearColor(glm::vec3{ 0.098f, 0.098f, 0.098f });
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


	m_gridMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Grid"));
	m_gridMaterial.SetBlendable(true);
	m_gridMaterial.SetBackfaceCulling(false);
	m_gridMaterial.SetDepthTest(false);

	m_shadowMapFbo.Bind();
	m_shadowMapFbo.Resize(2048, 2048);
	m_shadowMapFbo.Unbind();


	// 初始化UI相机
	m_camera4UI = new RenderCamera();
	m_camera4UI->set_clear_flag(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);// 不清理颜色, 只清理深度信息
	m_camera4UI->SetProjectionMode(ProjectionMode::ORTHOGRAPHIC);
	m_cameraPosition4UI = glm::vec3(glm::vec3(0, 0, -10));
	m_cameraRotation4UI = glm::quat(1,0,0,0);
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
	auto next = glm::quat(glm::vec3(glm::radians(60.0) * p_deltaTime, glm::radians(30.0) * p_deltaTime, 0));
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
	// 准备相机数据
	PrepareCamera();

	// 绘制场景
	RenderScene();

	// 绘制UI
	RenderUI();
}
void LitchiEditor::SceneView::RenderScene()
{
	Scene* scene = ApplicationEditor::Instance()->sceneManager->GetCurrentScene();;

	// 绑定Light的SSBO
	auto& ssbo = ApplicationEditor::Instance()->lightSSBO;
	FTransform* shadowLightTran = nullptr;
	ssbo->Bind(0);
	// 收集场景中的光
	std::vector<glm::mat4> lightMatrixArr;
	ssbo->SendBlocks(lightMatrixArr.data(), 0);
	scene->Foreach([&](GameObject* game_object) {
		if (game_object->GetActive() && game_object->GetLayer() != 0x02) {
			game_object->ForeachComponent([&](Component* component) {

				auto* lightComp = dynamic_cast<LightComponent*>(component);
				if (lightComp == nullptr) {
					return;
				}

				auto directionalLightTran = game_object->GetComponent<Transform>();
				lightMatrixArr.push_back(lightComp->GetData().GenerateMatrix(directionalLightTran->GetTransform()));

				if (lightComp->GetLight().type == static_cast<float>(Light::Type::DIRECTIONAL))
				{
					shadowLightTran = &directionalLightTran->GetTransform();
				}

				});
		}
		});
	ssbo->SendBlocks(lightMatrixArr.data(), sizeof(glm::mat4) * lightMatrixArr.size());


	// 先绘制一遍阴影
	glm::mat4 lightSpaceMatrix;
	if (shadowLightTran != nullptr)
	{
		glViewport(0, 0, 2048, 2048);
		m_shadowMapFbo.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		auto shadowMapShader = ApplicationEditor::Instance()->m_shadowMapShader;
		auto shadowMapShader4Skinned = ApplicationEditor::Instance()->m_shadowMapShader4Skinned;
	
		
		GLfloat near_plane = 0.0f, far_plane = 50.0f;

		// 计算光源的Projection
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		// 计算光源的View
		auto shadowLightPos = shadowLightTran->GetWorldPosition();
		auto shadowLightRotation = shadowLightTran->GetWorldRotation();
		glm::vec3 up = glm::normalize(shadowLightRotation) * glm::vec3(0, 1.0, 0);
		glm::vec3 forward = glm::normalize(shadowLightRotation) * glm::vec3(0, 0, -1.0);

		/*	DEBUG_LOG_INFO("---------------------------------");
			DEBUG_LOG_INFO("CalculateViewMatrix eulerRotation.X:{},eulerRotation.Y:{},eulerRotation.Z:{}", eulerRotation.x, eulerRotation.y, eulerRotation.z);
			DEBUG_LOG_INFO("CalculateViewMatrix forward.X:{},forward.Y:{},forward.Z:{}", forward.x, forward.y, forward.z);*/
		
		glm::mat4 lightView = glm::lookAt
		(
			glm::vec3(shadowLightPos.x, shadowLightPos.y, shadowLightPos.z),											// Position
			glm::vec3(shadowLightPos.x + forward.x, shadowLightPos.y + forward.y, shadowLightPos.z + forward.z),			// LookAt (Position + Forward)
			glm::vec3(up.x, up.y, up.z)																		// Up Vector
		);

		lightSpaceMatrix = lightProjection * lightView;
		shadowMapShader->Bind();
		shadowMapShader->SetUniformMat4("ubo_LightSpaceMatrix", lightSpaceMatrix);
		shadowMapShader->Unbind();
		shadowMapShader4Skinned->Bind();
		shadowMapShader4Skinned->SetUniformMat4("ubo_LightSpaceMatrix", lightSpaceMatrix);
		shadowMapShader4Skinned->Unbind();


		ApplicationEditor::Instance()->renderer->ApplyStateMask(63);
		// 遍历所有的物体,执行MeshRenderer的Render函数
		scene->Foreach([&](GameObject* game_object) {
			if (game_object->GetActive() && game_object->GetLayer() != 0x02) {
				game_object->ForeachComponent([&](Component* component) {
						auto* skinned_mesh_renderer = dynamic_cast<SkinnedMeshRenderer*>(component);
						if (skinned_mesh_renderer != nullptr) {
							shadowMapShader4Skinned->Bind();
							
							// 提交FinalTransform到GPU
							auto& finalTransformCacheArr = skinned_mesh_renderer->GetCurrentFinalTransformCacheArr();
							shadowMapShader4Skinned->SetUniformMat4("ubo_boneFinalMatrixArr[0]", *finalTransformCacheArr.data(), finalTransformCacheArr.size());

							skinned_mesh_renderer->RenderShadowMap();
							shadowMapShader4Skinned->Unbind();
							return;
						}

						auto* mesh_renderer = dynamic_cast<MeshRenderer*>(component);
						if (mesh_renderer != nullptr) {
							shadowMapShader->Bind();
							mesh_renderer->RenderShadowMap();
							shadowMapShader->Unbind();
							return;
						}
					});


			}
			});
		glCullFace(GL_BACK);
		m_shadowMapFbo.Unbind();

	}

	auto [winWidth, winHeight] = GetSafeSize();
	glViewport(0, 0, winWidth, winHeight);

	// 绑定FBO
	m_fbo.Bind();

	// 获取当前需要渲染的相机 和 场景
	RenderCamera* render_camera = m_camera;
	render_camera->Clear();
	// RenderGrid(m_cameraPosition, glm::vec3(0.098f, 0.898f, 0.098f));
	ApplicationEditor::Instance()->editorRenderer->RenderGrid(m_cameraPosition, m_gridColor);

	// 遍历所有的物体,执行MeshRenderer的Render函数
	scene->Foreach([&](GameObject* game_object) {
		if (game_object->GetActive() && game_object->GetLayer() != 0x02) {
			game_object->ForeachComponent([&](Component* component) {
				auto* mesh_renderer = dynamic_cast<MeshRenderer*>(component);
				if (mesh_renderer == nullptr) {
					return;
				}
				mesh_renderer->Render(render_camera,&lightSpaceMatrix, &m_shadowMapFbo);
				});
		}
		});

	// 解绑FBO
	m_fbo.Unbind();

	ssbo->Unbind();
}

void LitchiEditor::SceneView::RenderUI()
{
	m_cameraPosition4UI = glm::vec3(glm::vec3(0, 0, -10));
	m_cameraRotation4UI = glm::quat(1, 0, 0, 0);
	auto [winWidth, winHeight] = GetSafeSize();
	m_camera4UI->SetSize(winWidth / 2.0f);
	m_camera4UI->SetNear(-100);
	m_camera4UI->SetFar(100);
	m_camera4UI->CacheMatrices(winWidth, winHeight, m_cameraPosition4UI, m_cameraRotation4UI);

	// 绑定相机参数
	auto& engineUBO = *ApplicationEditor::Instance()->engineUBO.get();
	size_t offset = sizeof(glm::mat4); // We skip the model matrix (Which is a special case, modified every draw calls)
	engineUBO.SetSubData(m_camera4UI->GetViewMatrix(), std::ref(offset));
	engineUBO.SetSubData(m_camera4UI->GetProjectionMatrix(), std::ref(offset));
	engineUBO.SetSubData(m_cameraPosition4UI, std::ref(offset));

	Scene* scene = ApplicationEditor::Instance()->sceneManager->GetCurrentScene();

	// 绑定FBO
	m_fbo.Bind();

	RenderCamera* render_camera = m_camera4UI;
	render_camera->Clear();

	// 遍历所有物体, 绘制UI
	// 遍历所有的物体,执行MeshRenderer的Render函数
	scene->Foreach([&](GameObject* game_object) {
		if (game_object->GetActive() && game_object->GetLayer() == 0x02) {
			game_object->ForeachComponent([&](Component* component) {
				auto* mesh_renderer = dynamic_cast<MeshRenderer*>(component);
				if (mesh_renderer == nullptr) {
					return;
				}
				mesh_renderer->Render(render_camera, nullptr, &m_shadowMapFbo);
				});
		}
		});

	// 解绑FBO
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
	glm::mat4 modelMatrix = glm::translate(glm::vec3{ p_viewPos.x, 0.0f, p_viewPos.z })* glm::scale(glm::vec3{ gridSize * 2.0f, 1.f, gridSize * 2.0f });
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
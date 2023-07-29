
#include "Editor/include/Core/EditorRenderer.h"

#include <gtx/quaternion.hpp>

#include "Editor/include/ApplicationEditor.h"
#include "Editor/include/Core/GizmoBehaviour.h"
#include "Runtime/Function/Framework/Component/Physcis/box_collider.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Physcis/sphere_collider.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

const glm::vec3 DEBUG_BOUNDS_COLOR		= { 1.0f, 0.0f, 0.0f };
const glm::vec3 LIGHT_VOLUME_COLOR		= { 1.0f, 1.0f, 0.0f };
const glm::vec3 COLLIDER_COLOR			= { 0.0f, 1.0f, 0.0f };
const glm::vec3 FRUSTUM_COLOR			= { 1.0f, 1.0f, 1.0f };

LitchiEditor::EditorRenderer::EditorRenderer()
{
	
	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::STENCIL_TEST, true);
	ApplicationEditor::Instance()->renderer->SetStencilOperations(LitchiRuntime::EOperation::KEEP, LitchiRuntime::EOperation::KEEP, LitchiRuntime::EOperation::REPLACE);
	ApplicationEditor::Instance()->renderer->SetStencilAlgorithm(LitchiRuntime::EComparaisonAlgorithm::ALWAYS, 1, 0xFF);

	InitMaterials();

	//ApplicationEditor::Instance()->renderer->RegisterModelMatrixSender([this](const glm::mat4 & p_modelMatrix)
	//{
	//	ApplicationEditor::Instance()->engineUBO->SetSubData(glm::mat4::Transpose(p_modelMatrix), 0);
	//});

	//ApplicationEditor::Instance()->renderer->RegisterUserMatrixSender([this](const glm::mat4 & p_userMatrix)
	//{
	//	ApplicationEditor::Instance()->engineUBO->SetSubData
	//	(
	//		p_userMatrix, 

	//		// UBO layout offset
	//		sizeof(glm::mat4) +
	//		sizeof(glm::mat4) +
	//		sizeof(glm::mat4) +
	//		sizeof(glm::vec3) +
	//		sizeof(float)
	//	);
	//});
}

void LitchiEditor::EditorRenderer::InitMaterials()
{
	/* Default Material */
	m_defaultMaterial.SetShader(ApplicationEditor::Instance()->shaderManager->GetResource("Engine\\Shaders\\Standard.glsl"));
	m_defaultMaterial.Set("u_Diffuse", glm::vec4(1.f, 1.f, 1.f, 1.f));
	m_defaultMaterial.Set("u_Shininess", 100.0f);
	m_defaultMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);

	/* Empty Material */
	m_emptyMaterial.SetShader(ApplicationEditor::Instance()->shaderManager->GetResource("Engine\\Shaders\\Unlit.glsl"));
	m_emptyMaterial.Set("u_Diffuse", glm::vec4(1.f, 0.f, 1.f, 1.0f));
	m_emptyMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);

	/* Grid Material */
	m_gridMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Grid"));
	m_gridMaterial.SetBlendable(true);
	m_gridMaterial.SetBackfaceCulling(false);
	m_gridMaterial.SetDepthTest(false);

	/* Camera Material */
	m_cameraMaterial.SetShader(ApplicationEditor::Instance()->shaderManager->GetResource("Engine\\Shaders\\Lambert.glsl"));
	m_cameraMaterial.Set("u_Diffuse", glm::vec4(0.0f, 0.3f, 0.7f, 1.0f));
	m_cameraMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);

	/* Light Material */
	m_lightMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Billboard"));
	m_lightMaterial.Set("u_Diffuse", glm::vec4(1.f, 1.f, 0.5f, 0.5f));
	m_lightMaterial.SetBackfaceCulling(false);
	m_lightMaterial.SetBlendable(true);
	m_lightMaterial.SetDepthTest(false);

	/* Stencil Fill Material */
	m_stencilFillMaterial.SetShader(ApplicationEditor::Instance()->shaderManager->GetResource("Engine\\Shaders\\Unlit.glsl"));
	m_stencilFillMaterial.SetBackfaceCulling(true);
	m_stencilFillMaterial.SetDepthTest(false);
	m_stencilFillMaterial.SetColorWriting(false);
	m_stencilFillMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);

	/* Texture Material */
	m_textureMaterial.SetShader(ApplicationEditor::Instance()->shaderManager->GetResource("Engine\\Shaders\\Unlit.glsl"));
	m_textureMaterial.Set("u_Diffuse", glm::vec4(1.f, 1.f, 1.f, 1.f));
	m_textureMaterial.SetBackfaceCulling(false);
	m_textureMaterial.SetBlendable(true);
	m_textureMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);

	/* Outline Material */
	m_outlineMaterial.SetShader(ApplicationEditor::Instance()->shaderManager->GetResource("Engine\\Shaders\\Unlit.glsl"));
	m_outlineMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);
	m_outlineMaterial.SetDepthTest(false);

	/* Gizmo Arrow Material */
	m_gizmoArrowMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Gizmo"));
	m_gizmoArrowMaterial.SetGPUInstances(3);
	m_gizmoArrowMaterial.Set("u_IsBall", false);
	m_gizmoArrowMaterial.Set("u_IsPickable", false);

	/* Gizmo Ball Material */
	m_gizmoBallMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Gizmo"));
	m_gizmoBallMaterial.Set("u_IsBall", true);
	m_gizmoBallMaterial.Set("u_IsPickable", false);

	/* Gizmo Pickable Material */
	m_gizmoPickingMaterial.SetShader(ApplicationEditor::Instance()->editorResources->GetShader("Gizmo"));
	m_gizmoPickingMaterial.SetGPUInstances(3);
	m_gizmoPickingMaterial.Set("u_IsBall", false);
	m_gizmoPickingMaterial.Set("u_IsPickable", true);

	/* Picking Material */
	m_actorPickingMaterial.SetShader(ApplicationEditor::Instance()->shaderManager->GetResource("Engine\\Shaders\\Unlit.glsl"));
	m_actorPickingMaterial.Set("u_Diffuse", glm::vec4(1.f, 1.f, 1.f, 1.0f));
	m_actorPickingMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);
	m_actorPickingMaterial.SetFrontfaceCulling(false);
	m_actorPickingMaterial.SetBackfaceCulling(false);
}

void LitchiEditor::EditorRenderer::PreparePickingMaterial(LitchiRuntime::GameObject& p_actor, Resource::Material& p_material)
{
	uint32_t actorID = static_cast<uint32_t>(p_actor.id);

	auto bytes = reinterpret_cast<uint8_t*>(&actorID);
	auto color = glm::vec4{ bytes[0] / 255.0f, bytes[1] / 255.0f, bytes[2] / 255.0f, 1.0f };

	p_material.Set("u_Diffuse", color);
}

glm::mat4 LitchiEditor::EditorRenderer::CalculateCameraModelMatrix(LitchiRuntime::GameObject& p_actor)
{
	auto translation = glm::translate(p_actor.GetComponent<Transform>()->GetWorldPosition());
	auto rotation = glm::toMat4(p_actor.GetComponent<Transform>()->GetWorldRotation());

	return translation * rotation;
}

void LitchiEditor::EditorRenderer::RenderScene(const glm::vec3& p_cameraPosition, const RenderCamera& p_camera, const Frustum* p_customFrustum)
{
	/* Render the actors */
	/*ApplicationEditor::Instance()->lightSSBO->Bind(0);
	ApplicationEditor::Instance()->renderer->RenderScene(*ApplicationEditor::Instance()->sceneManager.GetCurrentScene(), p_cameraPosition, p_camera, p_customFrustum, &m_emptyMaterial);
	ApplicationEditor::Instance()->lightSSBO->Unbind();*/
}

void LitchiEditor::EditorRenderer::RenderSceneForActorPicking()
{
	//auto& scene = *ApplicationEditor::Instance()->sceneManager->GetCurrentScene();

	///* Render models */
	//for (auto modelRenderer : scene.GetFastAccessComponents().modelRenderers)
	//{
	//	auto& actor = modelRenderer->owner;

	//	if (actor.IsActive())
	//	{
	//		if (auto model = modelRenderer->GetModel())
	//		{
	//			if (auto materialRenderer = modelRenderer->owner.GetComponent<OvCore::ECS::Components::CMaterialRenderer>())
	//			{
	//				const OvCore::ECS::Components::CMaterialRenderer::MaterialList& materials = materialRenderer->GetMaterials();
	//				const auto& modelMatrix = actor->GetComponent<Transform>()->GetWorldMatrix();

	//				PreparePickingMaterial(actor, m_actorPickingMaterial);

	//				for (auto mesh : model->GetMeshes())
	//				{
	//					Resource::Material* material = nullptr;

	//					if (mesh->GetMaterialIndex() < MAX_MATERIAL_COUNT)
	//					{
	//						material = materials.at(mesh->GetMaterialIndex());
	//						if (!material || !material->GetShader())
	//							material = &m_emptyMaterial;
	//					}

	//					if (material)
	//					{
	//						m_actorPickingMaterial.SetBackfaceCulling(material->HasBackfaceCulling());
	//						m_actorPickingMaterial.SetFrontfaceCulling(material->HasFrontfaceCulling());
	//						m_actorPickingMaterial.SetColorWriting(material->HasColorWriting());
	//						m_actorPickingMaterial.SetDepthTest(material->HasDepthTest());
	//						m_actorPickingMaterial.SetDepthWriting(material->HasDepthWriting());

	//						ApplicationEditor::Instance()->renderer->DrawMesh(*mesh, m_actorPickingMaterial, &modelMatrix);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	///* Render cameras */
	//for (auto camera : ApplicationEditor::Instance()->sceneManager.GetCurrentScene()->GetFastAccessComponents().cameras)
	//{
	//	auto& actor = camera->owner;

	//	if (actor.IsActive())
	//	{
	//		PreparePickingMaterial(actor, m_actorPickingMaterial);
	//		auto& model = *ApplicationEditor::Instance()->editorResources->GetModel("Camera");
	//		auto modelMatrix = CalculateCameraModelMatrix(actor);

	//		ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(model, m_actorPickingMaterial, &modelMatrix);
	//	}
	//}

	///* Render lights */
	//if (Settings::EditorSettings::LightBillboardScale > 0.001f)
	//{
	//	ApplicationEditor::Instance()->renderer->Clear(false, true, false);

	//	m_lightMaterial.SetDepthTest(true);
	//	m_lightMaterial.Set<float>("u_Scale", EditorSettings::LightBillboardScale * 0.1f);
	//	m_lightMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", nullptr);

	//	for (auto light : ApplicationEditor::Instance()->sceneManager.GetCurrentScene()->GetFastAccessComponents().lights)
	//	{
	//		auto& actor = light->owner;

	//		if (actor.IsActive())
	//		{
	//			PreparePickingMaterial(actor, m_lightMaterial);
	//			auto& model = *ApplicationEditor::Instance()->editorResources->GetModel("Vertical_Plane");
	//			auto modelMatrix = glm::translate(actor.GetComponent<Transform>()->GetWorldPosition());
	//			ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(model, m_lightMaterial, &modelMatrix);
	//		}
	//	}
	//}
}

void LitchiEditor::EditorRenderer::RenderUI()
{
	ApplicationEditor::Instance()->uiManager->Render();
}

void LitchiEditor::EditorRenderer::RenderCameras()
{
	/*for (auto camera : ApplicationEditor::Instance()->sceneManager->GetCurrentScene()->GetFastAccessComponents().cameras)
	{
		auto& actor = camera->owner;

		if (actor.IsActive())
		{
			auto& model = *ApplicationEditor::Instance()->editorResources->GetModel("Camera");
			auto modelMatrix = CalculateCameraModelMatrix(actor);
			
			ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(model, m_cameraMaterial, &modelMatrix);
		}
	}*/
}

void LitchiEditor::EditorRenderer::RenderLights()
{
	/*using namespace OvMaths;

	m_lightMaterial.SetDepthTest(false);
	m_lightMaterial.Set<float>("u_Scale", Settings::EditorSettings::LightBillboardScale * 0.1f);

	for (auto light : ApplicationEditor::Instance()->sceneManager.GetCurrentScene()->GetFastAccessComponents().lights)
	{
		auto& actor = light->owner;

		if (actor.IsActive())
		{
			auto& model = *ApplicationEditor::Instance()->editorResources->GetModel("Vertical_Plane");
			auto modelMatrix = glm::mat4::Translation(actor->GetComponent<Transform>()->GetWorldPosition());

			LitchiRuntime::Texture* texture = nullptr;

			switch (static_cast<LitchiRuntime::Entities::Light::Type>(static_cast<int>(light->GetData().type)))
			{
			case LitchiRuntime::Entities::Light::Type::POINT:				texture = ApplicationEditor::Instance()->editorResources->GetTexture("Bill_Point_Light");			break;
			case LitchiRuntime::Entities::Light::Type::SPOT:				texture = ApplicationEditor::Instance()->editorResources->GetTexture("Bill_Spot_Light");				break;
			case LitchiRuntime::Entities::Light::Type::DIRECTIONAL:		texture = ApplicationEditor::Instance()->editorResources->GetTexture("Bill_Directional_Light");		break;
			case LitchiRuntime::Entities::Light::Type::AMBIENT_BOX:		texture = ApplicationEditor::Instance()->editorResources->GetTexture("Bill_Ambient_Box_Light");		break;
			case LitchiRuntime::Entities::Light::Type::AMBIENT_SPHERE:	texture = ApplicationEditor::Instance()->editorResources->GetTexture("Bill_Ambient_Sphere_Light");	break;
			}

			const auto& lightColor = light->GetColor();
			m_lightMaterial.Set<LitchiRuntime::Texture*>("u_DiffuseMap", texture);
			m_lightMaterial.Set<OvMaths::glm::vec4>("u_Diffuse", OvMaths::glm::vec4(lightColor.x, lightColor.y, lightColor.z, 0.75f));
			ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(model, m_lightMaterial, &modelMatrix);
		}
	}*/
}

void LitchiEditor::EditorRenderer::RenderGizmo(const glm::vec3& p_position, const glm::quat& p_rotation, LitchiEditor::EGizmoOperation p_operation, bool p_pickable, int p_highlightedAxis)
{
	glm::mat4 model = glm::translate(p_position) * glm::toMat4(glm::normalize(p_rotation));

	LitchiRuntime::Model* arrowModel = nullptr;

	if (!p_pickable)
	{
		glm::mat4 sphereModel = model * glm::scale(glm::vec3{ 0.1f, 0.1f, 0.1f });
		ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(*ApplicationEditor::Instance()->editorResources->GetModel("Sphere"), m_gizmoBallMaterial, &sphereModel);
		m_gizmoArrowMaterial.Set("u_HighlightedAxis", p_highlightedAxis);

		switch (p_operation)
		{
		case LitchiEditor::EGizmoOperation::TRANSLATE:
			arrowModel = ApplicationEditor::Instance()->editorResources->GetModel("Arrow_Translate");
			break;
		case LitchiEditor::EGizmoOperation::ROTATE:
			arrowModel = ApplicationEditor::Instance()->editorResources->GetModel("Arrow_Rotate");
			break;
		case LitchiEditor::EGizmoOperation::SCALE:
			arrowModel = ApplicationEditor::Instance()->editorResources->GetModel("Arrow_Scale");
			break;
		}
	}
	else
	{
		arrowModel = ApplicationEditor::Instance()->editorResources->GetModel("Arrow_Picking");
	}

	if (arrowModel)
	{
		ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(*arrowModel, p_pickable ? m_gizmoPickingMaterial : m_gizmoArrowMaterial, &model);
	}
}

void LitchiEditor::EditorRenderer::RenderModelToStencil(const glm::mat4& p_worldMatrix, LitchiRuntime::Model& p_model)
{
	ApplicationEditor::Instance()->renderer->SetStencilMask(0xFF);
	ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(p_model, m_stencilFillMaterial, &p_worldMatrix);
	ApplicationEditor::Instance()->renderer->SetStencilMask(0x00);
}

void LitchiEditor::EditorRenderer::RenderModelOutline(const glm::mat4& p_worldMatrix, LitchiRuntime::Model& p_model, float p_width)
{
	ApplicationEditor::Instance()->renderer->SetStencilAlgorithm(LitchiRuntime::EComparaisonAlgorithm::NOTEQUAL, 1, 0xFF);
	ApplicationEditor::Instance()->renderer->SetRasterizationMode(LitchiRuntime::ERasterizationMode::LINE);
	ApplicationEditor::Instance()->renderer->SetRasterizationLinesWidth(p_width);
	ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(p_model, m_outlineMaterial, &p_worldMatrix);
	ApplicationEditor::Instance()->renderer->SetRasterizationLinesWidth(1.f);
	ApplicationEditor::Instance()->renderer->SetRasterizationMode(LitchiRuntime::ERasterizationMode::FILL);
	ApplicationEditor::Instance()->renderer->SetStencilAlgorithm(LitchiRuntime::EComparaisonAlgorithm::ALWAYS, 1, 0xFF);
}

void LitchiEditor::EditorRenderer::RenderActorOutlinePass(LitchiRuntime::GameObject& p_actor, bool p_toStencil, bool p_isSelected)
{
	//float outlineWidth = p_isSelected ? 5.0f : 2.5f;

	//m_outlineMaterial.Set("u_Diffuse", p_isSelected ? glm::vec4(1.f, 0.7f, 0.f, 1.0f) : glm::vec4(1.f, 1.f, 0.f, 1.0f));

	//if (p_actor.active())
	//{
	//	/* Render static mesh outline and bounding spheres */
	//	if (auto modelRenderer = p_actor.GetComponent<MeshFilter>(); modelRenderer && modelRenderer->GetModel())
	//	{
	//		if (p_toStencil)
	//			RenderModelToStencil(p_actor.GetComponent<Transform>()->GetWorldMatrix(), *modelRenderer->GetModel());
	//		else
	//			RenderModelOutline(p_actor.GetComponent<Transform>()->GetWorldMatrix(), *modelRenderer->GetModel(), outlineWidth);

	//		if (p_isSelected && Settings::EditorSettings::ShowGeometryBounds && !p_toStencil)
	//		{
	//			RenderBoundingSpheres(*modelRenderer);
	//		}
	//	}

	//	/* Render camera component outline */
	//	if (auto cameraComponent = p_actor.GetComponent<Camera>(); cameraComponent)
	//	{
	//		auto model = CalculateCameraModelMatrix(p_actor);

	//		if (p_toStencil)
	//			RenderModelToStencil(model, *ApplicationEditor::Instance()->editorResources->GetModel("Camera"));
	//		else
	//			RenderModelOutline(model, *ApplicationEditor::Instance()->editorResources->GetModel("Camera"), outlineWidth);

	//		if (p_isSelected && !p_toStencil)
	//		{
	//			RenderCameraFrustum(*cameraComponent);
	//		}
	//	}

	//	if (p_isSelected && !p_toStencil)
	//	{
	//		/* Render the actor collider */
	//		if (p_actor.GetComponent<OvCore::ECS::Components::CPhysicalObject>())
	//		{
	//			RenderActorCollider(p_actor);
	//		}

	//		/* Render the actor ambient light */
	//		if (auto ambientBoxComp = p_actor.GetComponent<OvCore::ECS::Components::CAmbientBoxLight>())
	//		{
	//			RenderAmbientBoxVolume(*ambientBoxComp);
	//		}

	//		if (auto ambientSphereComp = p_actor.GetComponent<OvCore::ECS::Components::CAmbientSphereLight>())
	//		{
	//			RenderAmbientSphereVolume(*ambientSphereComp);
	//		}

	//		if (OvEditor::Settings::EditorSettings::ShowLightBounds)
	//		{
	//			if (auto light = p_actor.GetComponent<LightComponent>())
	//			{
	//				RenderLightBounds(*light);
	//			}
	//		}
	//	}

	//	for (auto& child : p_actor.GetChildren())
	//	{
	//		RenderActorOutlinePass(*child, p_toStencil, p_isSelected);
	//	}
	//}
}

//void DrawFrustumLines(LitchiRuntime::Core::ShapeDrawer& p_drawer,
//                      const glm::vec3& pos,
//                      const glm::vec3& forward,
//                      float near,
//                      const float far,
//                      const glm::vec3& a,
//                      const glm::vec3& b,
//                      const glm::vec3& c,
//                      const glm::vec3& d,
//                      const glm::vec3& e,
//                      const glm::vec3& f,
//                      const glm::vec3& g,
//                      const glm::vec3& h)
//{
//    // Convenient lambda to draw a frustum line
//    auto draw = [&](const glm::vec3& p_start, const glm::vec3& p_end, const float planeDistance)
//    {
//        auto offset = pos + forward * planeDistance;
//        auto start = offset + p_start;
//        auto end = offset + p_end;
//        p_drawer.DrawLine(start, end, FRUSTUM_COLOR);
//    };
//
//    // Draw near plane
//    draw(a, b, near);
//    draw(b, d, near);
//    draw(d, c, near);
//    draw(c, a, near);
//
//    // Draw far plane
//    draw(e, f, far);
//    draw(f, h, far);
//    draw(h, g, far);
//    draw(g, e, far);
//
//    // Draw lines between near and far planes
//    draw(a + forward * near, e + forward * far, 0);
//    draw(b + forward * near, f + forward * far, 0);
//    draw(c + forward * near, g + forward * far, 0);
//    draw(d + forward * near, h + forward * far, 0);
//}

void LitchiEditor::EditorRenderer::RenderCameraPerspectiveFrustum(std::pair<uint16_t, uint16_t>& p_size, Camera& p_camera)
{
    /*const auto& owner = p_camera.owner;
    auto& camera = p_camera.R();

    const auto& cameraPos = owner.transform.GetWorldPosition();
    const auto& cameraRotation = owner.transform.GetWorldRotation();
    const auto& cameraForward = owner.transform.GetWorldForward();

    camera.CacheMatrices(p_size.first, p_size.second, cameraPos, cameraRotation);
    const auto proj = glm::translate(camera.GetProjectionMatrix());
    const auto near = camera.GetNear();
    const auto far = camera.GetFar();

    const auto nLeft = near * (proj.data[2] - 1.0f) / proj.data[0];
    const auto nRight = near * (1.0f + proj.data[2]) / proj.data[0];
    const auto nTop = near * (1.0f + proj.data[6]) / proj.data[5];
    const auto nBottom = near * (proj.data[6] - 1.0f) / proj.data[5];

    const auto fLeft = far * (proj.data[2] - 1.0f) / proj.data[0];
    const auto fRight = far * (1.0f + proj.data[2]) / proj.data[0];
    const auto fTop = far * (1.0f + proj.data[6]) / proj.data[5];
    const auto fBottom = far * (proj.data[6] - 1.0f) / proj.data[5];

    auto a = cameraRotation * glm::vec3{ nLeft, nTop, 0 };
    auto b = cameraRotation * glm::vec3{ nRight, nTop, 0 };
    auto c = cameraRotation * glm::vec3{ nLeft, nBottom, 0 };
    auto d = cameraRotation * glm::vec3{ nRight, nBottom, 0 };
    auto e = cameraRotation * glm::vec3{ fLeft, fTop, 0 };
    auto f = cameraRotation * glm::vec3{ fRight, fTop, 0 };
    auto g = cameraRotation * glm::vec3{ fLeft, fBottom, 0 };
    auto h = cameraRotation * glm::vec3{ fRight, fBottom, 0 };

    DrawFrustumLines(*ApplicationEditor::Instance()->shapeDrawer, cameraPos, cameraForward, near, far, a, b, c, d, e, f, g, h);*/
}

void LitchiEditor::EditorRenderer::RenderCameraOrthographicFrustum(std::pair<uint16_t, uint16_t>& p_size, Camera& p_camera)
{
   /* auto& owner = p_camera.owner;
    auto& camera = p_camera.GetCamera();
    const auto ratio = p_size.first / static_cast<float>(p_size.second);

    const auto& cameraPos = owner.transform.GetWorldPosition();
    const auto& cameraRotation = owner.transform.GetWorldRotation();
    const auto& cameraForward = owner.transform.GetWorldForward();

    const auto near = camera.GetNear();
    const auto far = camera.GetFar();
    const auto size = p_camera.GetSize();

    const auto right = ratio * size;
    const auto left = -right;
    const auto top = size;
    const auto bottom = -top;

    const auto a = cameraRotation * glm::vec3{ left, top, 0 };
    const auto b = cameraRotation * glm::vec3{ right, top, 0 };
    const auto c = cameraRotation * glm::vec3{ left, bottom, 0 };
    const auto d = cameraRotation * glm::vec3{ right, bottom, 0 };

    DrawFrustumLines(*ApplicationEditor::Instance()->shapeDrawer, cameraPos, cameraForward, near, far, a, b, c, d, a, b, c, d);*/
}

void LitchiEditor::EditorRenderer::RenderCameraFrustum(Camera& p_camera)
{
  /*  auto& gameView = EDITOR_PANEL(GameView, "Game View");
    auto gameViewSize = gameView.GetSafeSize();

    if (gameViewSize.first == 0 || gameViewSize.second == 0)
    {
        gameViewSize = { 16, 9 };
    }

    switch (p_camera.GetProjectionMode())
    {
    case LitchiRuntime::EProjectionMode::ORTHOGRAPHIC:
        RenderCameraOrthographicFrustum(gameViewSize, p_camera);
        break;
    
    case LitchiRuntime::EProjectionMode::PERSPECTIVE:
        RenderCameraPerspectiveFrustum(gameViewSize, p_camera);
        break;
    }*/
}

void LitchiEditor::EditorRenderer::RenderActorCollider(GameObject& p_actor)
{
	bool depthTestBackup = ApplicationEditor::Instance()->renderer->GetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST);
	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, false);

	/* Draw the box collider if any */
	if (auto boxColliderComponent = p_actor.GetComponent<BoxCollider>(); boxColliderComponent)
	{
		glm::quat rotation = p_actor.GetComponent<Transform>()->GetWorldRotation();
		glm::vec3 center = p_actor.GetComponent<Transform>()->GetWorldPosition();
		glm::vec3 colliderSize = boxColliderComponent->GetSize();
		glm::vec3 actorScale = p_actor.GetComponent<Transform>()->GetWorldScale();
		glm::vec3 halfSize = { colliderSize.x * actorScale.x, colliderSize.y * actorScale.y, colliderSize.z * actorScale.z };
		glm::vec3 size = halfSize * 2.f;
		
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, -halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ -halfSize.x, -halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ -halfSize.x, +halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, -halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ -halfSize.x, +halfSize.y, -halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, -halfSize.y, +halfSize.z }, center	+ rotation * glm::vec3{ -halfSize.x, +halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ +halfSize.x, -halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, -halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ +halfSize.x, halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, +halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ +halfSize.x, -halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, +halfSize.y, -halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ +halfSize.x, -halfSize.y, +halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, +halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, -halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, -halfSize.y, -halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, +halfSize.y, -halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, +halfSize.y, -halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, -halfSize.y, +halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, -halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * glm::vec3{ -halfSize.x, +halfSize.y, +halfSize.z }, center	+ rotation * glm::vec3{ +halfSize.x, +halfSize.y, +halfSize.z }, glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	}

	/* Draw the sphere collider if any */
	if (auto sphereColliderComponent = p_actor.GetComponent<SphereCollider>(); sphereColliderComponent)
	{
		glm::vec3 actorScale = p_actor.GetComponent<Transform>()->GetWorldScale();
		glm::quat rotation = p_actor.GetComponent<Transform>()->GetWorldRotation();
		glm::vec3 center = p_actor.GetComponent<Transform>()->GetWorldPosition();
		float radius = sphereColliderComponent->GetRadius() * std::max(std::max(std::max(actorScale.x, actorScale.y), actorScale.z), 0.0f);

		for (float i = 0; i <= 360.0f; i += 10.0f)
		{
			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } * radius), center + rotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } * radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } * radius), center + rotation * (glm::vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } * radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } * radius), center + rotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } * radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
		}
	}

	///* Draw the capsule collider if any */
	//if (auto capsuleColliderComponent = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalCapsule>(); capsuleColliderComponent)
	//{
	//	float radius = abs(capsuleColliderComponent->GetRadius() * std::max(std::max(p_actor.GetComponent<Transform>()->GetWorldScale().x, p_actor.GetComponent<Transform>()->GetWorldScale().z), 0.f));
	//	float height = abs(capsuleColliderComponent->GetHeight() * p_actor.GetComponent<Transform>()->GetWorldScale().y);
	//	float halfHeight = height / 2;

	//	glm::vec3 actorScale = p_actor.GetComponent<Transform>()->GetWorldScale();
	//	glm::quat rotation = p_actor.GetComponent<Transform>()->GetWorldRotation();
	//	glm::vec3 center = p_actor.GetComponent<Transform>()->GetWorldPosition();

	//	glm::vec3 hVec = { 0.0f, halfHeight, 0.0f };
	//	for (float i = 0; i < 360.0f; i += 10.0f)
	//	{
	//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (hVec + glm::vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *radius), center + rotation * (hVec + glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (-hVec + glm::vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *radius), center + rotation * (-hVec + glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);

	//		if (i < 180.f)
	//		{
	//			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (hVec + glm::vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } * radius), center + rotation * (hVec + glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } * radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (hVec + glm::vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } * radius), center + rotation * (hVec + glm::vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } * radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//		}
	//		else
	//		{
	//			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (-hVec + glm::vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } * radius), center + rotation * (-hVec + glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } * radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (-hVec + glm::vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } * radius), center + rotation * (-hVec + glm::vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } * radius), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//		}
	//	}

	//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ -radius, -halfHeight, 0.f }),	center + rotation * (glm::vec3{ -radius, +halfHeight, 0.f }), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ radius, -halfHeight, 0.f }),	center + rotation * (glm::vec3{ radius, +halfHeight, 0.f }), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ 0.f, -halfHeight, -radius }),	center + rotation * (glm::vec3{ 0.f, +halfHeight, -radius }), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ 0.f, -halfHeight, radius }),	center + rotation * (glm::vec3{ 0.f, +halfHeight, radius }), glm::vec3{ 0.f, 1.f, 0.f }, 1.f);
	//}

	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, depthTestBackup);
	ApplicationEditor::Instance()->renderer->SetRasterizationLinesWidth(1.0f);
}

void LitchiEditor::EditorRenderer::RenderLightBounds(LightComponent& p_light)
{
	bool depthTestBackup = ApplicationEditor::Instance()->renderer->GetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST);
	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, false);

	auto& data = p_light.GetData();

	glm::quat rotation = p_light.GetGameObject()->GetComponent<Transform>()->GetWorldRotation();
	glm::vec3 center = p_light.GetGameObject()->GetComponent<Transform>()->GetWorldPosition();
	float radius = data.GetEffectRange(p_light.GetGameObject()->GetComponent<Transform>()->GetTransform());

	if (!std::isinf(radius))
	{
		for (float i = 0; i <= 360.0f; i += 10.0f)
		{
			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } *radius), center + rotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } *radius), DEBUG_BOUNDS_COLOR, 1.f);
			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } *radius), center + rotation * (glm::vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } *radius), DEBUG_BOUNDS_COLOR, 1.f);
			ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *radius), center + rotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *radius), DEBUG_BOUNDS_COLOR, 1.f);
		}
	}

	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, depthTestBackup);
}

//void LitchiEditor::EditorRenderer::RenderAmbientBoxVolume(OvCore::ECS::Components::CAmbientBoxLight & p_ambientBoxLight)
//{
//	bool depthTestBackup = ApplicationEditor::Instance()->renderer->GetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST);
//	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, false);
//
//	auto& data = p_ambientBoxLight.GetData();
//
//	glm::mat4 model =
//		glm::mat4::Translation(p_ambientBoxLight.owner.transform.GetWorldPosition()) *
//		glm::mat4::Scaling({ data.constant * 2.f, data.linear * 2.f, data.quadratic * 2.f });
//
//	glm::vec3 center = p_ambientBoxLight.owner.transform.GetWorldPosition();
//	glm::vec3 size = { data.constant * 2.f, data.linear * 2.f, data.quadratic * 2.f };
//	glm::vec3 actorScale = p_ambientBoxLight.owner.transform.GetWorldScale();
//	glm::vec3 halfSize = size / 2.f;
//
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, -halfSize.y, -halfSize.z }, center + glm::vec3{ -halfSize.x, -halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, halfSize.y, -halfSize.z }, center + glm::vec3{ -halfSize.x, +halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, -halfSize.y, -halfSize.z }, center + glm::vec3{ -halfSize.x, +halfSize.y, -halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, -halfSize.y, +halfSize.z }, center + glm::vec3{ -halfSize.x, +halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ +halfSize.x, -halfSize.y, -halfSize.z }, center + glm::vec3{ +halfSize.x, -halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ +halfSize.x, halfSize.y, -halfSize.z }, center + glm::vec3{ +halfSize.x, +halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ +halfSize.x, -halfSize.y, -halfSize.z }, center + glm::vec3{ +halfSize.x, +halfSize.y, -halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ +halfSize.x, -halfSize.y, +halfSize.z }, center + glm::vec3{ +halfSize.x, +halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, -halfSize.y, -halfSize.z }, center + glm::vec3{ +halfSize.x, -halfSize.y, -halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, +halfSize.y, -halfSize.z }, center + glm::vec3{ +halfSize.x, +halfSize.y, -halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, -halfSize.y, +halfSize.z }, center + glm::vec3{ +halfSize.x, -halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//	ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + glm::vec3{ -halfSize.x, +halfSize.y, +halfSize.z }, center + glm::vec3{ +halfSize.x, +halfSize.y, +halfSize.z }, LIGHT_VOLUME_COLOR, 1.f);
//
//	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, depthTestBackup);
//}
//
//void LitchiEditor::EditorRenderer::RenderAmbientSphereVolume(OvCore::ECS::Components::CAmbientSphereLight & p_ambientSphereLight)
//{
//	bool depthTestBackup = ApplicationEditor::Instance()->renderer->GetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST);
//	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, false);
//
//	auto& data = p_ambientSphereLight.GetData();
//
//	glm::quat rotation = p_ambientSphereLight.owner.transform.GetWorldRotation();
//	glm::vec3 center = p_ambientSphereLight.owner.transform.GetWorldPosition();
//	float radius = data.constant;
//
//	for (float i = 0; i <= 360.0f; i += 10.0f)
//	{
//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } *radius), center + rotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } *radius), LIGHT_VOLUME_COLOR, 1.f);
//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } *radius), center + rotation * (glm::vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } *radius), LIGHT_VOLUME_COLOR, 1.f);
//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(center + rotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *radius), center + rotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *radius), LIGHT_VOLUME_COLOR, 1.f);
//	}
//
//	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, depthTestBackup);
//}

void LitchiEditor::EditorRenderer::RenderBoundingSpheres(MeshFilter& p_modelRenderer)
{
	bool depthTestBackup = ApplicationEditor::Instance()->renderer->GetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST);
	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, false);

	/* Draw the sphere collider if any */
	//if (auto model = p_modelRenderer.GetModel())
	//{
	//	auto* actor = p_modelRenderer.game_object();

	//	glm::vec3 actorScale = actor->GetComponent<Transform>()->GetWorldScale();
	//	glm::quat actorRotation = actor->GetComponent<Transform>()->GetWorldRotation();
	//	glm::vec3 actorPosition = actor->GetComponent<Transform>()->GetWorldPosition();

	//	const auto& modelBoundingsphere = 
	//		p_modelRenderer.GetFrustumBehaviour() == MeshFilter::EFrustumBehaviour::CULL_CUSTOM ?
	//		p_modelRenderer.GetCustomBoundingSphere() :
	//		model->GetBoundingSphere();

	//	float radiusScale = std::max(std::max(std::max(actorScale.x, actorScale.y), actorScale.z), 0.0f);
	//	float scaledRadius = modelBoundingsphere.radius * radiusScale;
	//	auto sphereOffset = glm::quat::RotatePoint(modelBoundingsphere.position, actorRotation) * radiusScale;

	//	glm::vec3 boundingSphereCenter = actorPosition + sphereOffset;

	//	for (float i = 0; i <= 360.0f; i += 10.0f)
	//	{
	//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(boundingSphereCenter + actorRotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } *scaledRadius), boundingSphereCenter + actorRotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } *scaledRadius), DEBUG_BOUNDS_COLOR, 1.f);
	//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(boundingSphereCenter + actorRotation * (glm::vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } *scaledRadius), boundingSphereCenter + actorRotation * (glm::vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } *scaledRadius), DEBUG_BOUNDS_COLOR, 1.f);
	//		ApplicationEditor::Instance()->shapeDrawer->DrawLine(boundingSphereCenter + actorRotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *scaledRadius), boundingSphereCenter + actorRotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *scaledRadius), DEBUG_BOUNDS_COLOR, 1.f);
	//	}

	//	if (p_modelRenderer.GetFrustumBehaviour() == MeshFilter::EFrustumBehaviour::CULL_MESHES)
	//	{
	//		const auto& meshes = model->GetMeshes();

	//		if (meshes.size() > 1) // One mesh would result into the same bounding sphere for mesh and model
	//		{
	//			for (auto mesh : meshes)
	//			{
	//				auto& meshBoundingSphere = mesh->GetBoundingSphere();
	//				float scaledRadius = meshBoundingSphere.radius * radiusScale;
	//				auto sphereOffset = glm::quat::RotatePoint(meshBoundingSphere.position, actorRotation) * radiusScale;

	//				glm::vec3 boundingSphereCenter = actorPosition + sphereOffset;

	//				for (float i = 0; i <= 360.0f; i += 10.0f)
	//				{
	//					ApplicationEditor::Instance()->shapeDrawer->DrawLine(boundingSphereCenter + actorRotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } *scaledRadius), boundingSphereCenter + actorRotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } *scaledRadius), DEBUG_BOUNDS_COLOR, 1.f);
	//					ApplicationEditor::Instance()->shapeDrawer->DrawLine(boundingSphereCenter + actorRotation * (glm::vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } *scaledRadius), boundingSphereCenter + actorRotation * (glm::vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } *scaledRadius), DEBUG_BOUNDS_COLOR, 1.f);
	//					ApplicationEditor::Instance()->shapeDrawer->DrawLine(boundingSphereCenter + actorRotation * (glm::vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *scaledRadius), boundingSphereCenter + actorRotation * (glm::vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *scaledRadius), DEBUG_BOUNDS_COLOR, 1.f);
	//				}
	//			}
	//		}
	//	}
	//}

	ApplicationEditor::Instance()->renderer->SetCapability(LitchiRuntime::ERenderingCapability::DEPTH_TEST, depthTestBackup);
	ApplicationEditor::Instance()->renderer->SetRasterizationLinesWidth(1.0f);
}

void LitchiEditor::EditorRenderer::RenderModelAsset(LitchiRuntime::Model& p_model)
{
	glm::mat4 model = glm::scale(glm::vec3{ 3.f, 3.f, 3.f });
	ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(p_model, m_defaultMaterial, &model);
}

void LitchiEditor::EditorRenderer::RenderTextureAsset(LitchiRuntime::Texture & p_texture)
{
	glm::mat4 model = glm::rotate(glm::scale(glm::vec3{ 5.f, 5.f, 5.f }), 90.f * 0.0174f,glm::vec3(1.0f,0,0));

	m_textureMaterial.Set<Texture*>("u_DiffuseMap", &p_texture);
	ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(*ApplicationEditor::Instance()->editorResources->GetModel("Plane"), m_textureMaterial, &model);
}

void LitchiEditor::EditorRenderer::RenderMaterialAsset(Resource::Material& p_material)
{
	glm::mat4 model = glm::scale(glm::vec3{ 3.f, 3.f, 3.f });
	ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(*ApplicationEditor::Instance()->editorResources->GetModel("Sphere"), p_material, &model, &m_emptyMaterial);
}

void LitchiEditor::EditorRenderer::RenderGrid(const glm::vec3& p_viewPos, const glm::vec3& p_color)
{
    constexpr float gridSize = 5000.0f;

    glm::mat4 model = glm::translate(glm::vec3{ p_viewPos.x, 0.0f, p_viewPos.z }) * glm::scale(glm::vec3{ gridSize * 2.0f, 1.f, gridSize * 2.0f });
	m_gridMaterial.Set("u_Color", p_color);
	ApplicationEditor::Instance()->renderer->DrawModelWithSingleMaterial(*ApplicationEditor::Instance()->editorResources->GetModel("Plane"), m_gridMaterial, &model);

    ApplicationEditor::Instance()->shapeDrawer->DrawLine(glm::vec3(-gridSize + p_viewPos.x, 0.0f, 0.0f), glm::vec3(gridSize + p_viewPos.x, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
    ApplicationEditor::Instance()->shapeDrawer->DrawLine(glm::vec3(0.0f, -gridSize + p_viewPos.y, 0.0f), glm::vec3(0.0f, gridSize + p_viewPos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
    ApplicationEditor::Instance()->shapeDrawer->DrawLine(glm::vec3(0.0f, 0.0f, -gridSize + p_viewPos.z), glm::vec3(0.0f, 0.0f, gridSize + p_viewPos.z), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
}

void LitchiEditor::EditorRenderer::UpdateLights(Scene& p_scene)
{
	/*auto lightMatrices = ApplicationEditor::Instance()->renderer->FindLightMatrices(p_scene);
	ApplicationEditor::Instance()->lightSSBO->SendBlocks<glm::mat4>(lightMatrices.data(), lightMatrices.size() * sizeof(glm::mat4));*/
}

void LitchiEditor::EditorRenderer::UpdateLightsInFrustum(Scene& p_scene, const Frustum& p_frustum)
{
	/*auto lightMatrices = ApplicationEditor::Instance()->renderer->FindLightMatricesInFrustum(p_scene, p_frustum);
	ApplicationEditor::Instance()->lightSSBO->SendBlocks<glm::mat4>(lightMatrices.data(), lightMatrices.size() * sizeof(glm::mat4));*/
}

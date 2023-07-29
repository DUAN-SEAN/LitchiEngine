
#include <glad/glad.h>
#include "mesh_renderer.h"
#include <rttr/registration>
#include <gtx/transform2.hpp>
#include "mesh_filter.h"
#include "gtx/quaternion.hpp"
#include "Runtime/Core/App/application_base.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Renderer/gpu_resource_mapper.h"
#include "Runtime/Function/Renderer/render_camera.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"

namespace LitchiRuntime
{
	MeshRenderer::MeshRenderer() {

	}

	MeshRenderer::~MeshRenderer() {

	}

	void MeshRenderer::SetMaterial(Resource::Material* material) {
		material = material;
	}

	void MeshRenderer::Render(RenderCamera* renderCamera, glm::mat4 const* lightVPMat, Framebuffer4Depth* shadowMapFBO)
	{
		// 通过MeshFilter获取当前的Mesh
		auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
		if(meshFilter==nullptr)
		{
			return;
		}

		// 计算模型矩阵
		auto transform = GetGameObject()->GetComponent<Transform>();
		glm::mat4 modelMatrix = transform->GetWorldMatrix();// 旋转->缩放->平移 TRS

		auto* extraMesh =meshFilter->GetExtraMesh();
		if(extraMesh != nullptr)
		{
			ApplicationBase::Instance()->renderer->DrawMesh(*extraMesh, *material, &modelMatrix, lightVPMat, shadowMapFBO);
		}else
		{
			auto model = meshFilter->GetModel();
			if (model == nullptr)
			{
				return;
			}

			if (model->GetMeshes().size() <= meshFilter->GetMeshIndex())
			{
				return;
			}

			auto mesh = model->GetMeshes().at(meshFilter->GetMeshIndex());
			if (mesh == nullptr)
			{
				return;
			}

			// vp矩阵绑定？

			// 调用Draw执行绘制
			// 内部将设置模型矩阵到UBO中
			ApplicationBase::Instance()->renderer->DrawMesh(*mesh, *material, &modelMatrix, lightVPMat, shadowMapFBO);
		}

	}

	void MeshRenderer::RenderShadowMap()
	{
		// 通过MeshFilter获取当前的Mesh
		auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
		if (meshFilter == nullptr)
		{
			return;
		}
		auto model = meshFilter->GetModel();
		if (model == nullptr)
		{
			return;
		}

		if(model->GetMeshes().size() <= meshFilter->GetMeshIndex())
		{
			return;
		}

		auto mesh = model->GetMeshes().at(meshFilter->GetMeshIndex());
		if (mesh == nullptr)
		{
			return;
		}

		// 计算模型矩阵
		auto transform = GetGameObject()->GetComponent<Transform>();
		glm::mat4 modelMatrix = transform->GetWorldMatrix();// 旋转->缩放->平移 TRS
		
		ApplicationBase::Instance()->engineUBO->SetSubData(modelMatrix, 0);

		// 调用Draw执行绘制
		// 内部将设置模型矩阵到UBO中
		ApplicationBase::Instance()->renderer->Draw(*mesh);
	}

	void MeshRenderer::PostResourceLoaded()
	{
		DEBUG_LOG_INFO("PostResourceLoaded Path {}", materialPath);

		if(materialPath.empty())
		{
			return;
		}

		material = ApplicationBase::Instance()->materialManager->GetResource(materialPath);
		if(material != nullptr)
		{
			material->PostResourceLoaded();
		}
	}
	void MeshRenderer::PostResourceModify()
	{
		DEBUG_LOG_INFO("PostResourceModify Path {}", materialPath);

		if (materialPath.empty())
		{
			return;
		}

		material = ApplicationBase::Instance()->materialManager->GetResource(materialPath);
		if (material != nullptr)
		{
			material->PostResourceModify();
		}
	}
}



#include "MeshRenderer.h"

#include <glad/glad.h>
#include <rttr/registration>
#include <gtx/transform2.hpp>

#include "MeshFilter.h"
#include "gtx/quaternion.hpp"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

namespace LitchiRuntime
{
	MeshRenderer::MeshRenderer() {

	}

	MeshRenderer::~MeshRenderer() {

	}

	//void MeshRenderer::Render(RenderCamera* renderCamera, glm::mat4 const* lightVPMat, Framebuffer4Depth* shadowMapFBO)
	//{
	//	// 通过MeshFilter获取当前的Mesh
	//	auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
	//	if(meshFilter==nullptr)
	//	{
	//		return;
	//	}

	//	// 计算模型矩阵
	//	auto transform = GetGameObject()->GetComponent<Transform>();
	//	glm::mat4 modelMatrix = transform->GetWorldMatrix();// 旋转->缩放->平移 TRS

	//	auto* extraMesh =meshFilter->GetExtraMesh();
	//	if(extraMesh != nullptr)
	//	{
	//		ApplicationBase::Instance()->renderer->DrawMesh(*extraMesh, *material, &modelMatrix, lightVPMat, shadowMapFBO);
	//	}else
	//	{
	//		auto model = meshFilter->GetModel();
	//		if (model == nullptr)
	//		{
	//			return;
	//		}

	//		if (model->GetMeshes().size() <= meshFilter->GetMeshIndex())
	//		{
	//			return;
	//		}

	//		auto mesh = model->GetMeshes().at(meshFilter->GetMeshIndex());
	//		if (mesh == nullptr)
	//		{
	//			return;
	//		}

	//		// vp矩阵绑定？

	//		// 调用Draw执行绘制
	//		// 内部将设置模型矩阵到UBO中
	//		ApplicationBase::Instance()->renderer->DrawMesh(*mesh, *material, &modelMatrix, lightVPMat, shadowMapFBO);
	//	}

	//}

	void MeshRenderer::RenderShadowMap()
	{
		//// 通过MeshFilter获取当前的Mesh
		//auto meshFilter = GetGameObject()->GetComponent<MeshFilter>();
		//if (meshFilter == nullptr)
		//{
		//	return;
		//}
		//auto model = meshFilter->GetModel();
		//if (model == nullptr)
		//{
		//	return;
		//}

		//if(model->GetMeshes().size() <= meshFilter->GetMeshIndex())
		//{
		//	return;
		//}

		//auto mesh = model->GetMeshes().at(meshFilter->GetMeshIndex());
		//if (mesh == nullptr)
		//{
		//	return;
		//}

		//// 计算模型矩阵
		//auto transform = GetGameObject()->GetComponent<Transform>();
		//glm::mat4 modelMatrix = transform->GetWorldMatrix();// 旋转->缩放->平移 TRS
		//
		//ApplicationBase::Instance()->engineUBO->SetSubData(modelMatrix, 0);

		//// 调用Draw执行绘制
		//// 内部将设置模型矩阵到UBO中
		//ApplicationBase::Instance()->renderer->Draw(*mesh);
	}

	void MeshRenderer::PostResourceLoaded()
	{
		DEBUG_LOG_INFO("PostResourceLoaded Path {}", materialPath);

		// todo

		/*if(materialPath.empty())
		{
			return;
		}

		material = ApplicationBase::Instance()->materialManager->GetResource(materialPath);
		if(material != nullptr)
		{
			material->PostResourceLoaded();
		}*/
	}
	void MeshRenderer::PostResourceModify()
	{
		DEBUG_LOG_INFO("PostResourceModify Path {}", materialPath);

		// todo

		/*if (materialPath.empty())
		{
			return;
		}

		material = ApplicationBase::Instance()->materialManager->GetResource(materialPath);
		if (material != nullptr)
		{
			material->PostResourceModify();
		}*/
	}

	// all functions (set/load) resolve to this
	Material* MeshRenderer::SetMaterial(Material* material)
	{
		SP_ASSERT(material != nullptr);

		// in order for the component to guarantee serialization/deserialization, we cache the material
		// Material* _material = ResourceCache::Cache(material);

		m_material = material;

		// set to false otherwise material won't serialize/deserialize
		m_material_default = false;

		return material;
	}

	Material* MeshRenderer::SetMaterial(const string& file_path)
	{
		// load the material
		//auto material = make_shared<Material>();
		auto material = new Material();
		if (!material->LoadFromFile(file_path))
		{
			DEBUG_LOG_WARN("Failed to load material from \"{}\"", file_path.c_str());
			return nullptr;
		}

		// set it as the current material
		return SetMaterial(material);
	}

	void MeshRenderer::SetDefaultMaterial()
	{
		// todo:

		//m_material_default = true;
		//const string data_dir = ResourceCache::GetDataDirectory() + "\\";
		//FileSystem::CreateDirectory(data_dir);

		//// create material
		//shared_ptr<Material> material = make_shared<Material>();
		//material->SetResourceFilePath(ResourceCache::GetProjectDirectory() + "standard" + EXTENSION_MATERIAL); // Set resource file path so it can be used by the resource cache
		//material->SetProperty(MaterialProperty::CanBeEdited, 0.0f);
		//material->SetProperty(MaterialProperty::UvTilingX, 10.0f);
		//material->SetProperty(MaterialProperty::UvTilingY, 10.0f);
		//material->SetProperty(MaterialProperty::ColorR, 1.0f);
		//material->SetProperty(MaterialProperty::ColorG, 1.0f);
		//material->SetProperty(MaterialProperty::ColorB, 1.0f);
		//material->SetProperty(MaterialProperty::ColorA, 1.0f);

		//// set default texture
		//material->SetTexture(MaterialTexture::Color, Renderer::GetStandardTexture(Renderer_StandardTexture::Checkerboard));

		//// set material
		//SetMaterial(material);
		//m_material_default = true;

		m_material_default = false;
	}

	string MeshRenderer::GetMaterialName() const
	{
		return m_material ? m_material->GetObjectName() : "";
	}
}


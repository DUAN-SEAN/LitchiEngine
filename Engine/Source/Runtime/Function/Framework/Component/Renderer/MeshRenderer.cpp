
#include "MeshRenderer.h"
#include "MeshFilter.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"

namespace LitchiRuntime
{
	MeshRenderer::MeshRenderer() {

	}

	MeshRenderer::~MeshRenderer() {

	}
	
	void MeshRenderer::Update()
	{
		if (m_material != nullptr)
		{
			m_material->Tick();
		}
	}
	
	void MeshRenderer::PostResourceLoaded()
	{
		DEBUG_LOG_INFO("PostResourceLoaded Path {}", materialPath);

		// todo

		if (materialPath.empty())
		{
			return;
		}

		m_material = ApplicationBase::Instance()->materialManager->GetResource(materialPath);
	}
	void MeshRenderer::PostResourceModify()
	{
		DEBUG_LOG_INFO("PostResourceModify Path {}", materialPath);

		// todo

		if (materialPath.empty())
		{
			return;
		}

		m_material = ApplicationBase::Instance()->materialManager->GetResource(materialPath);
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

		m_material = ApplicationBase::Instance()->materialManager->LoadResource("Engine\\Materials\\Standard4Phong.mat");

		m_material_default = true;
	}

	string MeshRenderer::GetMaterialName() const
	{
		return m_material ? m_material->GetObjectName() : "";
	}
}


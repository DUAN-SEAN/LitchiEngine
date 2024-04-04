
#include "Runtime/Core/pch.h"
#include "MeshRenderer.h"
#include "MeshFilter.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"

namespace LitchiRuntime
{
	MeshRenderer::MeshRenderer() {

	}

	MeshRenderer::~MeshRenderer() {

	}
	
	void MeshRenderer::OnUpdate()
	{
		if (m_material != nullptr)
		{
			m_material->Tick();
		}
	}

	void MeshRenderer::OnEditorUpdate()
	{
		if (m_material != nullptr)
		{
			m_material->Tick();
		}
	}
	
	void MeshRenderer::PostResourceLoaded()
	{
		PostResourceModify();
	}
	void MeshRenderer::PostResourceModify()
	{
		if (m_materialPath.empty() || m_materialPath == "Empty")
		{
			return;
		}

		auto material = ApplicationBase::Instance()->materialManager->GetResource(m_materialPath);
		if (!material)
		{
			DEBUG_LOG_WARN("Failed to load material from \"{}\"", m_materialPath);
			return;
		}
		SetMaterial(material);
	}

	// all functions (set/load) resolve to this
	Material* MeshRenderer::SetMaterial(Material* material)
	{
		LC_ASSERT(material != nullptr);

		// in order for the component to guarantee serialization/deserialization, we cache the material
		// Material* _material = ResourceCache::Cache(material);

		m_material = material;

		// set to false otherwise material won't serialize/deserialize
		m_useDefaultMaterial = false;

		m_materialPath = material->GetResourceFilePathAsset();

		return material;
	}

	Material* MeshRenderer::SetMaterial(const string& filePath)
	{
		// load the material
		//auto material = make_shared<Material>();
		auto material = ApplicationBase::Instance()->materialManager->GetResource(m_materialPath);
		if (!material)
		{
			DEBUG_LOG_WARN("Failed to load material from \"{}\"", filePath.c_str());
			return nullptr;
		}

		// set it as the current material
		return SetMaterial(material);
	}

	void MeshRenderer::SetDefaultMaterial()
	{
		auto material = ApplicationBase::Instance()->materialManager->LoadResource(":Materials/Standard4Phong.mat");
		SetMaterial(material);
		m_useDefaultMaterial = true;
	}

	string MeshRenderer::GetMaterialName() const
	{
		return m_material ? m_material->GetObjectName() : "";
	}
}


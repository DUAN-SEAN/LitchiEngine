
#include "rttr/registration"

#include "MaterialLoader.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Resource/asset_manager.h"

LitchiRuntime::Resource::Material* LitchiRuntime::Loaders::MaterialLoader::Create(const std::string & p_path)
{
	Resource::MaterialRes* materialRes = new Resource::MaterialRes();
	if(AssetManager::LoadAsset(p_path, *materialRes))
	{
		Resource::Material* material = new Resource::Material();
		material->materialRes = materialRes;
		material->path = p_path;
		material->PostResourceLoaded();

		return material;
	}
	
	return nullptr;
}

void LitchiRuntime::Loaders::MaterialLoader::Reload(LitchiRuntime::Resource::Material& p_material, const std::string& p_path)
{
	if (AssetManager::LoadAsset(p_path, *p_material.materialRes))
	{
		p_material.PostResourceLoaded();
		
	}
}

void LitchiRuntime::Loaders::MaterialLoader::Save(LitchiRuntime::Resource::Material& p_material, const std::string& p_path)
{
	p_material.RebuildResourceFromMemory();
	if(!AssetManager::SaveAsset(*p_material.materialRes, p_path))
	{
		DEBUG_LOG_ERROR("Save Fail path:{}",p_path);
	}
}

bool LitchiRuntime::Loaders::MaterialLoader::Destroy(LitchiRuntime::Resource::Material*& p_material)
{
	if (p_material)
	{
		delete p_material;
		p_material = nullptr;

		return true;
	}

	return false;
}
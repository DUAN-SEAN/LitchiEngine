
#include "Runtime/Core/pch.h"
#include "MaterialManager.h"

namespace LitchiRuntime
{
	Material* MaterialManager::CreateResource(const std::string& p_path)
	{
		std::string realPath = GetRealPath(p_path);

		Material* material = new Material();
		if (!material->LoadFromFile(realPath))
		{
			delete material;
			return nullptr;
		}

		return material;
	}

	void MaterialManager::DestroyResource(Material* p_resource)
	{
		delete p_resource;
		// Loaders::MaterialLoader::Destroy(p_resource);
	}

	void MaterialManager::ReloadResource(Material* p_resource, const std::string& p_path)
	{
		std::string realPath = GetRealPath(p_path);
		if (!p_resource->LoadFromFile(realPath))
		{
			DEBUG_LOG_ERROR("ReloadResource Fail path:{}", p_path);
		}
	}
	
}


#include "MaterialManager.h"

namespace LitchiRuntime
{
	Material* MaterialManager::CreateResource(const std::string& p_path)
	{
		std::string realPath = GetRealPath(p_path);

		Material* material = new Material();
		if (material)
		{
			if (material->LoadFromFile(realPath))
			{
				// material->SetResourceFilePath(p_path);
			}
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
		// Loaders::MaterialLoader::Reload(*p_resource, p_path);
	}
	
}

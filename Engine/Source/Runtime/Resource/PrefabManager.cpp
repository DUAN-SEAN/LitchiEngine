
#include "Runtime/Core/pch.h"
#include "PrefabManager.h"

namespace LitchiRuntime
{
	Prefab* PrefabManager::CreateResource(const std::string& p_path)
	{
		std::string realPath = GetRealPath(p_path);

		Prefab* prefab = new Prefab();
		if (prefab)
		{
			if (prefab->LoadFromFile(realPath))
			{
				// material->SetResourceFilePath(p_path);
			}
		}

		return prefab;
	}

	void PrefabManager::DestroyResource(LitchiRuntime::Prefab* p_resource)
	{
		delete p_resource;
		//Loaders::TextureLoader::Destroy(p_resource);
	}

	void PrefabManager::ReloadResource(LitchiRuntime::Prefab* p_resource, const std::string& p_path)
	{
		/*std::string realPath = GetRealPath(p_path);

		auto [min, mag, mipmap] = GetAssetMetadata(realPath);

		Loaders::TextureLoader::Reload(*p_resource, realPath, min, mag, mipmap);*/
	}

	Prefab* PrefabManager::CreatePrefab(const std::string& p_path)
	{
		Prefab* prefab = new Prefab(p_path);
		RegisterResource(p_path, prefab);

		return prefab;
	}
}

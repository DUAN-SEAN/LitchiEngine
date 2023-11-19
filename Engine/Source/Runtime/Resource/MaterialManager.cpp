
#include "MaterialManager.h"

#include "Runtime/Function/Renderer/Resources/Loaders/MaterialLoader.h"

LitchiRuntime::Material* LitchiRuntime::MaterialManager::CreateResource(const std::string & p_path)
{
	std::string realPath = GetRealPath(p_path);

	LitchiRuntime::Material* material = new Material();
	if (material)
	{
		if(material->LoadFromFile(realPath))
		{
			// material->SetResourceFilePath(p_path);
		}
	}

	return material;
}

void LitchiRuntime::MaterialManager::DestroyResource(LitchiRuntime::Material* p_resource)
{
	// Loaders::MaterialLoader::Destroy(p_resource);
}

void LitchiRuntime::MaterialManager::ReloadResource(LitchiRuntime::Material* p_resource, const std::string& p_path)
{
	// Loaders::MaterialLoader::Reload(*p_resource, p_path);
}

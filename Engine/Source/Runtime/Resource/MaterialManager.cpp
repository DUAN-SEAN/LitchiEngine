
#include "MaterialManager.h"

#include "Runtime/Function/Renderer/Resources/Loaders/MaterialLoader.h"

LitchiRuntime::Resource::Material* LitchiRuntime::MaterialManager::CreateResource(const std::string & p_path)
{
	std::string realPath = GetRealPath(p_path);

	LitchiRuntime::Resource::Material* material = Loaders::MaterialLoader::Create(realPath);
	if (material)
	{
		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(material) + offsetof(LitchiRuntime::Resource::Material, path)) = p_path; // Force the resource path to fit the given path
	}

	return material;
}

void LitchiRuntime::MaterialManager::DestroyResource(LitchiRuntime::Resource::Material* p_resource)
{
	Loaders::MaterialLoader::Destroy(p_resource);
}

void LitchiRuntime::MaterialManager::ReloadResource(LitchiRuntime::Resource::Material* p_resource, const std::string& p_path)
{
	Loaders::MaterialLoader::Reload(*p_resource, p_path);
}

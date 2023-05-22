
#include "ShaderManager.h"

#include "Runtime/Function/Renderer/shader.h"
#include "Runtime/Function/Renderer/Resources/Loaders/ShaderLoader.h"

LitchiRuntime::Resource::Shader* LitchiRuntime::ShaderManager::CreateResource(const std::string & p_path)
{
	std::string realPath = GetRealPath(p_path);
	LitchiRuntime::Resource::Shader* shader = Loaders::ShaderLoader::Create(realPath);
	if (shader)
		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(shader) + offsetof(LitchiRuntime::Resource::Shader, path)) = p_path; // Force the resource path to fit the given path

	return shader;
}

void LitchiRuntime::ShaderManager::DestroyResource(LitchiRuntime::Resource::Shader* p_resource)
{
	Loaders::ShaderLoader::Destroy(p_resource);
}

void LitchiRuntime::ShaderManager::ReloadResource(LitchiRuntime::Resource::Shader* p_resource, const std::string& p_path)
{
	Loaders::ShaderLoader::Recompile(*p_resource, p_path);
}

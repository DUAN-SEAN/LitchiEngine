
#include "Runtime/Core/pch.h"
#include "ShaderManager.h"

LitchiRuntime::MaterialShader* LitchiRuntime::ShaderManager::CreateResource(const std::string & p_path)
{
	std::string realPath = GetRealPath(p_path);
	LitchiRuntime::MaterialShader* shader = new MaterialShader();
	if (shader)
	{
		shader->LoadFromFile(realPath);

		//*reinterpret_cast<std::string*>(reinterpret_cast<char*>(shader) + offsetof(LitchiRuntime::Shader, path)) = p_path; // Force the resource path to fit the given path
	}

	return shader;
}

void LitchiRuntime::ShaderManager::DestroyResource(LitchiRuntime::MaterialShader* p_resource)
{
	delete p_resource;
	// Loaders::ShaderLoader::Destroy(p_resource);
}

void LitchiRuntime::ShaderManager::ReloadResource(LitchiRuntime::MaterialShader* p_resource, const std::string& p_path)
{
	// Loaders::ShaderLoader::Recompile(*p_resource, p_path);
}

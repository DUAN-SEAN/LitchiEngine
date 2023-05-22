
#pragma once

#include "AResourceManager.h"
#include "Runtime/Function/Renderer/Resources/Shader.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of shaders
	*/
	class ShaderManager : public AResourceManager<LitchiRuntime::Resource::Shader>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual LitchiRuntime::Resource::Shader* CreateResource(const std::string & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(LitchiRuntime::Resource::Shader* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(LitchiRuntime::Resource::Shader* p_resource, const std::string& p_path) override;
	};
}

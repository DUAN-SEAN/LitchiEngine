
#pragma once

#include "AResourceManager.h"
#include "Runtime/Function/Renderer/Rendering/Material.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of shaders
	*/
	class ShaderManager : public AResourceManager<LitchiRuntime::MaterialShader>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual LitchiRuntime::MaterialShader* CreateResource(const std::string & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(LitchiRuntime::MaterialShader* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(LitchiRuntime::MaterialShader* p_resource, const std::string& p_path) override;
	};
}

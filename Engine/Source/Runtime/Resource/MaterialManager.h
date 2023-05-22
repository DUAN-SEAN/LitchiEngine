
#pragma once

#include "AResourceManager.h"
#include "Runtime/Function/Renderer/Resources/Material.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of materials
	*/
	class MaterialManager : public AResourceManager<LitchiRuntime::Resource::Material>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual LitchiRuntime::Resource::Material* CreateResource(const std::string & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(LitchiRuntime::Resource::Material* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(LitchiRuntime::Resource::Material* p_resource, const std::string& p_path) override;
	};
}

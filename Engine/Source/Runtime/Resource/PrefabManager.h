
#pragma once
#include "AResourceManager.h"
#include "Runtime/Function/Prefab/Prefab.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of textures
	*/
	class PrefabManager : public AResourceManager<Prefab>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual Prefab* CreateResource(const std::string& p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(Prefab* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(Prefab* p_resource, const std::string& p_path) override;
	public:
		Prefab* CreatePrefab(const std::string& p_path);
	};
}

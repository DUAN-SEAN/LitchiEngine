
#pragma once

#include "AResourceManager.h"
#include "Runtime/Function/Renderer/Resources/Model.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of models
	*/
	class ModelManager : public AResourceManager<Model>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual Model* CreateResource(const std::string & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(Model* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(Model* p_resource, const std::string& p_path) override;
	};
}

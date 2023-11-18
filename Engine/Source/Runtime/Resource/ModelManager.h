
#pragma once

#include "AResourceManager.h"
#include "Runtime/Function/Renderer/Rendering/Mesh.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of models
	*/
	class ModelManager : public AResourceManager<Mesh>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual Mesh* CreateResource(const std::string & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(Mesh* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(Mesh* p_resource, const std::string& p_path) override;
	};
}

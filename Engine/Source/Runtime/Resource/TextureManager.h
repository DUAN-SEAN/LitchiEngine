
#pragma once

#include "AResourceManager.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2D.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of textures
	*/
	class TextureManager : public AResourceManager<RHI_Texture2D>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual RHI_Texture2D* CreateResource(const std::string & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(RHI_Texture2D* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(RHI_Texture2D* p_resource, const std::string& p_path) override;
	};
}

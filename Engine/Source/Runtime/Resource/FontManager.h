
#pragma once

#include "AResourceManager.h"
#include "Runtime/Function/Renderer/Font/font.h"
#include "Runtime/Function/Renderer/Resources/Texture.h"

namespace LitchiRuntime
{
	/**
	* ResourceManager of textures
	*/
	class FontManager : public AResourceManager<Font>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual Font* CreateResource(const std::string & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(Font* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(Font* p_resource, const std::string& p_path) override;
	};
}

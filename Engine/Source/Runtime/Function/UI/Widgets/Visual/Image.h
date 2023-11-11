
#pragma once
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
#include "Runtime/Function/UI/Internal/TextureID.h"
#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Simple widget that display an image
	*/
	class Image : public LitchiRuntime::AWidget
	{
	public:
		/**
		* Constructor
		* @param p_textureID
		* @parma p_size
		*/
		Image(uint32_t p_textureID, const Vector2& p_size);
		Image(RHI_Texture* p_renderTarget, const Vector2& p_size);

		void UpdateRenderTarget(RHI_Texture* p_renderTarget) { renderTarget = p_renderTarget; }

	protected:
		void _Draw_Impl() override;

	public:

		TextureID textureID;
		RHI_Texture* renderTarget;
		Vector2 size;
	};
}

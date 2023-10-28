
#pragma once

#include "AButton.h"
#include "Runtime/Function/UI/Internal/TextureID.h"
#include  "Runtime/Function/Renderer/Rendering/Color.h"

namespace LitchiRuntime
{
	/**
	* Button widget with an image
	*/
	class ButtonImage : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_textureID
		* @param p_size
		*/
		ButtonImage(uint32_t p_textureID, const Vector2& p_size);

	protected:
		void _Draw_Impl() override;

	public:
		bool disabled = false;

		Color background = { 0, 0, 0, 0 };
		Color tint = { 1, 1, 1, 1 };

		TextureID textureID;
		Vector2 size;
	};
}


#pragma once
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

	protected:
		void _Draw_Impl() override;

	public:
		TextureID textureID;
		Vector2 size;
	};
}

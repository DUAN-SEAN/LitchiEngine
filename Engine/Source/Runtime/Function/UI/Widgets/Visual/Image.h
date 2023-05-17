
#pragma once
#include "glm.hpp"
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
		Image(uint32_t p_textureID, const glm::vec2& p_size);

	protected:
		void _Draw_Impl() override;

	public:
		TextureID textureID;
		glm::vec2 size;
	};
}

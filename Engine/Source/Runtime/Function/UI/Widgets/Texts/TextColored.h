
#pragma once

#include "Text.h"
#include "Runtime/Function/Renderer/Rendering/Color.h"
#include  "Runtime/Function/Renderer/Rendering/Color.h"

namespace LitchiRuntime
{
	/**
	* Widget to display text on a panel that can be colored
	*/
	class TextColored : public Text
	{
	public:
		/**
		* Constructor
		* @param p_content
		* @param p_color
		*/
		TextColored(const std::string& p_content = "", const Color& p_color = Color(1.0f, 1.0f, 1.0f, 1.0f));

	public:
		Color color;

	protected:
		virtual void _Draw_Impl() override;
	};
}

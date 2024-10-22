
#pragma once

#include <string>

#include "AButton.h"
#include  "Runtime/Function/Renderer/Rendering/Color.h"

namespace LitchiRuntime
{
	/**
	* Button widget of a single color (Color palette element)
	*/
	class ButtonColored : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_label
		* @param p_color
		* @param p_size
		* @param p_enableAlpha
		*/
		ButtonColored(const std::string& p_label = "", const Color& p_color = {1.0f,1.0f ,1.0f ,1.0f }, const Vector2& p_size = Vector2(0.f, 0.f), bool p_enableAlpha = true);

	protected:
		void _Draw_Impl() override;

	public:
		std::string label;
		Color color;
		Vector2 size;
		bool enableAlpha;
	};
}

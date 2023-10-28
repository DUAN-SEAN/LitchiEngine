
#pragma once

#include <string>

#include "AButton.h"
#include  "Runtime/Function/Renderer/Rendering/Color.h"

namespace LitchiRuntime
{
	/**
	* Simple button widget
	*/
	class Button : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_label
		* @param p_size
		* @param p_disabled
		*/
		Button(const std::string& p_label = "", const Vector2& p_size = Vector2(0.f, 0.f), bool p_disabled = false);

	protected:
		void _Draw_Impl() override;

	public:
		std::string label;
		Vector2 size;
		bool disabled = false;

		Color idleBackgroundColor{1.0f,1.0f ,1.0f ,1.0f };
		Color hoveredBackgroundColor{ 1.0f,1.0f ,1.0f ,1.0f };
		Color clickedBackgroundColor{ 1.0f,1.0f ,1.0f ,1.0f };

		Color textColor{ 1.0f,1.0f ,1.0f ,1.0f };
	};
}

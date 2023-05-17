
#pragma once

#include "glm.hpp"
#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Dummy widget that takes the given size as space in the panel
	*/
	class Dummy : public AWidget
	{
	public:
		/**
		* Constructor
		* @param p_size
		*/
		Dummy(const glm::vec2& p_size = glm::vec2( 0.0f, 0.0f ));

	protected:
		void _Draw_Impl() override;

	public:
		glm::vec2 size;
	};
}

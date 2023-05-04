/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <limits>

#include "core/type.hpp"
#include "Runtime/Function/UI/Widgets/DataWidget.h"

namespace LitchiRuntime
{
	/**
	* Base class for any plot widget
	*/
	class APlot : public DataWidget<std::vector<float>>
	{
	public:
		/**
		* Constructor
		* @param p_data
		* @param p_minScale
		* @param p_maxScale
		* @param p_size
		* @param p_overlay
		* @param p_label
		* @param p_forceHover
		*/
		APlot
		(
			const std::vector<float>& p_data = std::vector<float>(),
			float p_minScale = std::numeric_limits<float>::min(),
			float p_maxScale = std::numeric_limits<float>::max(),
			const glm::vec2& p_size = glm::vec2( 0.0f, 0.0f ),
			const std::string& p_overlay = "",
			const std::string& p_label = "",
			int p_forceHover = -1
		);

	protected:
		virtual void _Draw_Impl() override = 0;

	public:
		std::vector<float> data;
		float minScale;
		float maxScale;
		glm::vec2 size;
		std::string overlay;
		std::string label;
		int forceHover;
	};
}

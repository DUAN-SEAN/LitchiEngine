
#pragma once
#include "core/type.hpp"
#include "Runtime/Function/UI/ImGui/imgui.h"
#include "Runtime/Function/UI/Types/Color.h"

namespace LitchiRuntime
{
	/**
	* Handles imgui conversion to/from overload types
	*/
	class Converter
	{
	public:
		/**
		* Disabled constructor
		*/
		Converter() = delete;

		/**
		* Convert the given Color to ImVec4
		* @param p_value
		*/
		static ImVec4 ToImVec4(const Color& p_value);

		/**
		* Convert the given ImVec4 to Color
		* @param p_value
		*/
		static Color ToColor(const ImVec4& p_value);

		/**
		* Convert the given FVector2 to ImVec2
		* @param p_value
		*/
		static ImVec2 ToImVec2(const glm::vec2& p_value);

		/**
		* Convert the given ImVec2 to FVector2
		* @param p_value
		*/
		static glm::vec2 ToFVector2(const ImVec2& p_value);
	};
}

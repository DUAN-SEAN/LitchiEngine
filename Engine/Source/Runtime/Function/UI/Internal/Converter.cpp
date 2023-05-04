
#include "Converter.h"

ImVec4 LitchiRuntime::Converter::ToImVec4(const Color & p_value)
{
	return ImVec4(p_value.r, p_value.g, p_value.b, p_value.a);
}

LitchiRuntime::Color LitchiRuntime::Converter::ToColor(const ImVec4 & p_value)
{
	return LitchiRuntime::Color(p_value.x, p_value.y, p_value.z, p_value.w);
}

ImVec2 LitchiRuntime::Converter::ToImVec2(const glm::vec2 & p_value)
{
	return ImVec2(p_value.x, p_value.y);
}

glm::vec2 LitchiRuntime::Converter::ToFVector2(const ImVec2 & p_value)
{
	return glm::vec2(p_value.x, p_value.y);
}

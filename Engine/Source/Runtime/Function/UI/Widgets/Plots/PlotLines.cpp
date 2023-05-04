
#include "PlotLines.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::PlotLines::PlotLines
(
	const std::vector<float>& p_data,
	float p_minScale,
	float p_maxScale,
	const glm::vec2& p_size,
	const std::string& p_overlay,
	const std::string& p_label,
	int p_forceHover
) : APlot(p_data, p_minScale, p_maxScale, p_size, p_overlay, p_label, p_forceHover)
{
}

void LitchiRuntime::PlotLines::_Draw_Impl()
{
	ImGui::PlotLines((label + m_widgetID).c_str(), data.data(), static_cast<int>(data.size()), 0, overlay.c_str(), minScale, maxScale, Converter::ToImVec2(size), sizeof(float), forceHover);
}


#include "PlotHistogram.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::PlotHistogram::PlotHistogram
(
	const std::vector<float>& p_data,
	float p_minScale,
	float p_maxScale,
	const Vector2& p_size,
	const std::string& p_overlay,
	const std::string& p_label,
	int p_forceHover
) : APlot(p_data, p_minScale, p_maxScale, p_size, p_overlay, p_label, p_forceHover)
{
}

void LitchiRuntime::PlotHistogram::_Draw_Impl()
{
	// todo: ¡Ÿ ±◊¢ Õ
	// ImGui::PlotHistogram((label + m_widgetID).c_str(), data.data(), static_cast<int>(data.size()), 0, overlay.c_str(), minScale, maxScale, Converter::ToImVec2(size), sizeof(float), forceHover);
}

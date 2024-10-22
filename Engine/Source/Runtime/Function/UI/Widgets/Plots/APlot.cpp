
#include "Runtime/Core/pch.h"
#include "APlot.h"

LitchiRuntime::APlot::APlot
(
	const std::vector<float>& p_data,
	float p_minScale,
	float p_maxScale,
	const Vector2& p_size,
	const std::string& p_overlay,
	const std::string& p_label,
	int p_forceHover
) : DataWidget(data), data(p_data), minScale(p_minScale), maxScale(p_maxScale), size(p_size), overlay(p_overlay), label(p_label), forceHover(p_forceHover)
{
}
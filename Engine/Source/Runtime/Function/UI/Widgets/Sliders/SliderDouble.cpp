
#include "Runtime/Core/pch.h"
#include "SliderDouble.h"

LitchiRuntime::SliderDouble::SliderDouble
(
	double p_min,
	double p_max,
	double p_value,
	ESliderOrientation p_orientation,
	const std::string & p_label,
	const std::string & p_format
) : SliderSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double, p_min, p_max, p_value, p_orientation, p_label, p_format)
{
}
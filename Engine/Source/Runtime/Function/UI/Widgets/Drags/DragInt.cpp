
#include "Runtime/Core/pch.h"
#include "DragInt.h"

#include "Runtime/Function/UI/ImGui/imgui.h"

LitchiRuntime::DragInt::DragInt
(
	int p_min,
	int p_max,
	int p_value,
	float p_speed,
	const std::string& p_label,
	const std::string& p_format
) : DragSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, p_min, p_max, p_value, p_speed, p_label, p_format)
{
}


#include "InputInt32.h"

LitchiRuntime::InputInt32::InputInt32(int32_t p_defaultValue, int32_t p_step, int32_t p_fastStep, const std::string & p_label, const std::string & p_format, bool p_selectAllOnClick)
	: InputSingleScalar<int32_t>(ImGuiDataType_::ImGuiDataType_Float, p_defaultValue, p_step, p_fastStep, p_label, p_format, p_selectAllOnClick)
{
}

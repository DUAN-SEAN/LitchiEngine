
//= INCLUDES =====================
#include "Runtime/Core/pch.h"
#include "../RHI_Implementation.h"
#include "../RHI_InputLayout.h"
//================================

//==================
using namespace std;
//==================

namespace LitchiRuntime
{
    RHI_InputLayout::~RHI_InputLayout()
    {

    }

    bool RHI_InputLayout::_CreateResource(void* vertex_shader_blob)
    {
        return true;
    }
}

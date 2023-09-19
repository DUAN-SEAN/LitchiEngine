
#include "RHI_Object.h"

#include <cstdint>

namespace Spartan
{
    uint64_t g_id = 0;

    RHI_Object::RHI_Object()
    {
        m_object_id = GenerateObjectId();
    }
}

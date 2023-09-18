
//= INCLUDES ==================
#include "Runtime/Core/pch.h"
#include "RHI_PhysicalDevice.h"
//=============================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
    string PhysicalDevice::decode_driver_version(const uint32_t version)
    {
        char buffer[256];

        if (IsNvidia())
        {
            sprintf
            (
                buffer,
                "%d.%d.%d.%d",
                (version >> 22) & 0x3ff,
                (version >> 14) & 0x0ff,
                (version >> 6) & 0x0ff,
                (version) & 0x003f
            );

        }
        else if (IsIntel())
        {
            sprintf
            (
                buffer,
                "%d.%d",
                (version >> 14),
                (version) & 0x3fff
            );
        }
        else // Use Vulkan version conventions if vendor mapping is not available
        {
            sprintf
            (
                buffer,
                "%d.%d.%d",
                (version >> 22),
                (version >> 12) & 0x3ff,
                version & 0xfff
            );
        }

        return buffer;
    }
}


//= INCLUDES =================
#include "Runtime/Core/pch.h"
#include "RHI_DescriptorSet.h"
#include "RHI_Device.h"
//============================

namespace Spartan
{
    Spartan::RHI_DescriptorSet::RHI_DescriptorSet(const std::vector<RHI_Descriptor>& descriptors, RHI_DescriptorSetLayout* descriptor_set_layout, const char* name)
    {
       /* if (name)
        {
            m_object_name = name;
        }*/

        // allocate
        {
            RHI_Device::AllocateDescriptorSet(m_resource, descriptor_set_layout, descriptors);
            RHI_Device::SetResourceName(m_resource, RHI_Resource_Type::DescriptorSet, name);
        }

        Update(descriptors);
    }
}

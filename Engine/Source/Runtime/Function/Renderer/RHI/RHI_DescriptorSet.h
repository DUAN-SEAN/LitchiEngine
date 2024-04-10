
#pragma once

//= INCLUDES =================
#include "RHI_Descriptor.h"   
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace LitchiRuntime
{
    class LC_CLASS RHI_DescriptorSet : public Object
    {
    public:
        RHI_DescriptorSet() = default;
        RHI_DescriptorSet(const std::vector<RHI_Descriptor>& descriptors, RHI_DescriptorSetLayout* descriptor_set_layout, const char* name);
        ~RHI_DescriptorSet() = default;

        bool IsReferingToResource(void* resource);
        void* GetResource() { return m_resource; }

    private:
        void Update(const std::vector<RHI_Descriptor>& descriptors);

        std::vector<RHI_Descriptor> m_descriptors;
        void* m_resource = nullptr;
    };
}

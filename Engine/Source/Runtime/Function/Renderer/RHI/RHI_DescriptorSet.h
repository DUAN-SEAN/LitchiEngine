
#pragma once

//= INCLUDES =================
#include "RHI_Descriptor.h"   
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace Spartan
{
    class SP_CLASS RHI_DescriptorSet : public LitchiRuntime::Object
    {
    public:
        RHI_DescriptorSet() = default;
        RHI_DescriptorSet(const std::vector<RHI_Descriptor>& descriptors, RHI_DescriptorSetLayout* descriptor_set_layout, const char* name);
        ~RHI_DescriptorSet() = default;

        void* GetResource() { return m_resource; }

    private:
        void Update(const std::vector<RHI_Descriptor>& descriptors);

        void* m_resource = nullptr;
    };
}

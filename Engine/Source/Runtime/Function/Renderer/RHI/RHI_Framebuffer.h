
#pragma once

//= INCLUDES =================
#include "RHI_Definitions.h"
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace LitchiRuntime
{
    class LC_CLASS RHI_Framebuffer : public Object
    {
    public:
        RHI_Framebuffer(const char* name = nullptr);
        ~RHI_Framebuffer();
        
        void* GetRhiResource() { return m_rhi_resource; }
        
    private:
        
        void CreateResource();

        void* m_rhi_resource = nullptr;
    };
}

#pragma once

//= INCLUDES =================
#include "RHI_Definitions.h"
#include "RHI_Object.h"
//============================

namespace LitchiRuntime
{
    class SP_CLASS RHI_Framebuffer : public RHI_Object
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
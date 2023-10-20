
#pragma once

//= INCLUDES =================
#include <memory>
#include "RHI_Definitions.h"
#include "Runtime/Core/Meta/Reflection/object.h"
//============================

namespace LitchiRuntime
{
    class RHI_Sampler : public Object
    {
    public:
        RHI_Sampler(
            const RHI_Filter filter_min                         = RHI_Filter::Nearest,
            const RHI_Filter filter_mag                         = RHI_Filter::Nearest,
            const RHI_Filter filter_mipmap                      = RHI_Filter::Nearest,
            const RHI_Sampler_Address_Mode sampler_address_mode = RHI_Sampler_Address_Mode::Wrap,
            const RHI_Comparison_Function comparison_function   = RHI_Comparison_Function::Always,
            const float anisotropy                              = 0.0f,
            const bool comparison_enabled                       = false,
            const float mip_bias                                = 0.0f
            );
        ~RHI_Sampler();

        RHI_Filter GetFilterMin()                       const { return m_filter_min; }
        RHI_Filter GetFilterMag()                       const { return m_filter_mag; }
        RHI_Filter GetFilterMipmap()                    const { return m_filter_mipmap; }
        RHI_Sampler_Address_Mode GetAddressMode()       const { return m_sampler_address_mode; }
        RHI_Comparison_Function GetComparisonFunction() const { return m_comparison_function; }
        bool GetAnisotropyEnabled()                     const { return m_anisotropy != 0; }
        bool GetComparisonEnabled()                     const { return m_comparison_enabled; }
        void* GetRhiResource()                             const { return m_rhi_resource; }

    private:
        void CreateResource();

        RHI_Filter m_filter_min                         = RHI_Filter::Nearest;
        RHI_Filter m_filter_mag                         = RHI_Filter::Nearest;
        RHI_Filter m_filter_mipmap                      = RHI_Filter::Nearest;
        RHI_Sampler_Address_Mode m_sampler_address_mode = RHI_Sampler_Address_Mode::Wrap;
        RHI_Comparison_Function m_comparison_function   = RHI_Comparison_Function::Always;
        float m_anisotropy                              = 0;
        bool m_comparison_enabled                       = false;
        float m_mip_lod_bias                            = 0.0f;

        // RHI Resource
        void* m_rhi_resource = nullptr;
    };
}

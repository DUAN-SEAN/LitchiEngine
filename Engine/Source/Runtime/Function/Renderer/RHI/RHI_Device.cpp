

//= INCLUDES ==========
#include "Runtime/Core/pch.h"
#include "RHI_Device.h"
//=====================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
    // Device properties
    float RHI_Device::m_timestamp_period                       = 0;
    uint64_t RHI_Device::m_min_uniform_buffer_offset_alignment = 0;
    uint64_t RHI_Device::m_min_storage_buffer_offset_alignment = 0;
    uint32_t RHI_Device::m_max_texture_1d_dimension            = 0;
    uint32_t RHI_Device::m_max_texture_2d_dimension            = 0;
    uint32_t RHI_Device::m_max_texture_3d_dimension            = 0;
    uint32_t RHI_Device::m_max_texture_cube_dimension          = 0;
    uint32_t RHI_Device::m_max_texture_array_layers            = 0;
    uint32_t RHI_Device::m_max_push_constant_size              = 0;
    uint32_t RHI_Device::m_max_shading_rate_texel_size_x = 0;
    uint32_t RHI_Device::m_max_shading_rate_texel_size_y = 0;
    bool RHI_Device::m_is_shading_rate_supported = false;

    // Misc
    bool RHI_Device::m_wide_lines                          = false;
    uint32_t  RHI_Device::m_physical_device_index          = 0;
    static vector<PhysicalDevice> physical_devices;

    void RHI_Device::PhysicalDeviceRegister(const PhysicalDevice& physical_device)
    {
        physical_devices.emplace_back(physical_device);

        // Sort devices by type, discrete devices come first.
        sort(physical_devices.begin(), physical_devices.end(), [](const PhysicalDevice& adapter1, const PhysicalDevice& adapter2)
        {
            return adapter1.GetType() == RHI_PhysicalDevice_Type::Discrete;
        });

        // Sort devices by memory, in an ascending order. The type order will be maintained.
        sort(physical_devices.begin(), physical_devices.end(), [](const PhysicalDevice& adapter1, const PhysicalDevice& adapter2)
        {
            return adapter1.GetMemory() > adapter2.GetMemory() && adapter1.GetType() == adapter2.GetType();
        });

        // DEBUG_LOG_INFO("%s (%d MB)", physical_device.GetName().c_str(), physical_device.GetMemory());
    }

    PhysicalDevice* RHI_Device::GetPrimaryPhysicalDevice()
    {
        LC_ASSERT_MSG(physical_devices.size() != 0, "No physical devices detected");
        LC_ASSERT_MSG(m_physical_device_index < physical_devices.size(), "Index out of bounds");

        return &physical_devices[m_physical_device_index];
    }

    void RHI_Device::PhysicalDeviceSetPrimary(const uint32_t index)
    {
        m_physical_device_index = index;

        if (const PhysicalDevice* physical_device = GetPrimaryPhysicalDevice())
        {
            // DEBUG_LOG_INFO("%s (%d MB)", physical_device->GetName().c_str(), physical_device->GetMemory());
        }
    }
    //void RHI_Device::SetVariableRateShading(const RHI_CommandList* cmd_list, const bool enabled)
    //{
    //    if (!m_is_shading_rate_supported)
    //        return;

    //    // set the fragment shading rate state for the current pipeline
    //    VkExtent2D fragment_size = { 1, 1 };
    //    VkFragmentShadingRateCombinerOpKHR combiner_operatins[2];

    //    // The combiners determine how the different shading rate values for the pipeline, primitives and attachment are combined
    //    if (enabled)
    //    {
    //        // If shading rate from attachment is enabled, we set the combiner, so that the values from the attachment are used
    //        // Combiner for pipeline (A) and primitive (B) - Not used in this sample
    //        combiner_operatins[0] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
    //        // Combiner for pipeline (A) and attachment (B), replace the pipeline default value (fragment_size) with the fragment sizes stored in the attachment
    //        combiner_operatins[1] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
    //    }
    //    else
    //    {
    //        // If shading rate from attachment is disabled, we keep the value set via the dynamic state
    //        combiner_operatins[0] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
    //        combiner_operatins[1] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
    //    }

    //    functions::set_fragment_shading_rate(static_cast<VkCommandBuffer>(cmd_list->GetRhiResource()), &fragment_size, combiner_operatins);
    //}
    vector<PhysicalDevice>& RHI_Device::PhysicalDeviceGet()
    {
        return physical_devices;
    }

    bool RHI_Device::IsValidResolution(const uint32_t width, const uint32_t height)
    {
        return width  >= 0 && width  <= m_max_texture_2d_dimension &&
               height >= 0 && height <= m_max_texture_2d_dimension;
    }

    void RHI_Device::QueueWaitAll()
    {
        QueueWait(RHI_Queue_Type::Graphics);
        QueueWait(RHI_Queue_Type::Copy);
        QueueWait(RHI_Queue_Type::Compute);
    }
}

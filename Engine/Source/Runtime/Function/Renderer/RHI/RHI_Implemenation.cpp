
//= INCLUDES ==================
#include "Runtime/Core/pch.h"
#include "RHI_Implementation.h"
//=============================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    // API specific
#   if defined(API_GRAPHICS_D3D12)
    RHI_Api_Type  RHI_Context::api_type = RHI_Api_Type::D3d12;
    string        RHI_Context::api_type_str = "D3D12";
    ID3D12Device* RHI_Context::device = nullptr;
    #elif defined(API_GRAPHICS_VULKAN)

        RHI_Api_Type RHI_Context::api_type = RHI_Api_Type::Vulkan;
        string RHI_Context::api_version_str = "Vulkan";
        VkInstance RHI_Context::instance;
        VkPhysicalDevice RHI_Context::device_physical;
        VkDevice RHI_Context::device;

        vector<VkValidationFeatureEnableEXT> RHI_Context::validation_extensions;
        vector<const char*> RHI_Context::extensions_instance = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_swapchain_colorspace" };
        vector<const char*> RHI_Context::validation_layers = { "VK_LAYER_KHRONOS_validation" };
        vector<const char*> RHI_Context::extensions_device = { "VK_KHR_swapchain", "VK_EXT_memory_budget", "VK_KHR_fragment_shading_rate", "VK_EXT_hdr_metadata", "VK_EXT_robustness2" };
    #endif
    
    // API agnostic
    string RHI_Context::api_type_str;

    bool RHI_Context::validation;
    bool RHI_Context::gpu_markers;
    bool RHI_Context::gpu_profiling;
    bool RHI_Context::renderdoc;

    void RHI_Context::Initialize()
    {
     
        #ifdef _DEBUG
            /*validation = false;
            gpu_markers = false;
            gpu_profiling = false;
            renderdoc = false;*/
            validation    = true;
            gpu_markers   = false;
            gpu_profiling = false;
            renderdoc     = false;
        #else
            validation    = false;
            gpu_markers   = false;
            gpu_profiling = false;
            renderdoc     = false;
        #endif
    } 
}

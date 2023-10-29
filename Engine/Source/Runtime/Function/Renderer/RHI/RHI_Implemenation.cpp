
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
        ID3D12Device* RHI_Context::device;
    #elif defined(API_GRAPHICS_VULKAN)
        VkInstance RHI_Context::instance;
        VkPhysicalDevice RHI_Context::device_physical;
        VkDevice RHI_Context::device;
        vector<VkValidationFeatureEnableEXT> RHI_Context::validation_extensions;
        vector<const char*> RHI_Context::extensions_instance;
        vector<const char*> RHI_Context::validation_layers;
        vector<const char*> RHI_Context::extensions_device;
    #endif
    
    // API agnostic
    string RHI_Context::api_version_str;
    string RHI_Context::api_type_str;
    RHI_Api_Type RHI_Context::api_type = RHI_Api_Type::Undefined;
    bool RHI_Context::validation;
    bool RHI_Context::gpu_markers;
    bool RHI_Context::gpu_profiling;
    bool RHI_Context::renderdoc;

    void RHI_Context::Initialize()
    {
        #if defined(API_GRAPHICS_D3D12)
            api_type            = RHI_Api_Type::D3d12;
            api_type_str        = "D3D12";
            device              = nullptr;
        #elif defined(API_GRAPHICS_VULKAN)
            api_type            = RHI_Api_Type::Vulkan;
            api_type_str        = "Vulkan";
            instance            = nullptr;
            device_physical     = nullptr;
            device              = nullptr;
            extensions_instance = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_swapchain_colorspace" };
            validation_layers   = { "VK_LAYER_KHRONOS_validation" };
            extensions_device   = { "VK_KHR_swapchain", "VK_EXT_memory_budget", "VK_EXT_depth_clip_enable" };
            // hardware capability viewer: https://vulkan.gpuinfo.org/
        #endif
        
        #ifdef DEBUG
            validation    = true;
            gpu_markers   = true;
            gpu_profiling = true;
            renderdoc     = false;
        #else
           /* validation    = false;
            gpu_markers   = false;
            gpu_profiling = true;
            renderdoc     = false;*/

            validation = true;
            gpu_markers = true;
            gpu_profiling = true;
            renderdoc = false;
        #endif
    } 
}

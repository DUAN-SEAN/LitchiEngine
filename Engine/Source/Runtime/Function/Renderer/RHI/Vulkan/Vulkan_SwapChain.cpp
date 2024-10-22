
//= INCLUDES =====================
#include "Runtime/Core/pch.h"
#include "Runtime/Core/Window/Window.h"

#include "../RHI_Device.h"
#include "../RHI_SwapChain.h"
#include "../RHI_Implementation.h"
#include "../RHI_Semaphore.h"
#include "../RHI_CommandPool.h"
#include <vulkan/vulkan.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/RHI/RHI_Fence.h"

//LC_WARNINGS_OFF
//#include <SDL_vulkan.h>
//LC_WARNINGS_ON
//================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
//============================

namespace LitchiRuntime
{
    namespace
    {
        VkColorSpaceKHR get_color_space(const RHI_Format format)
        {
            VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;                                                       // SDR
            color_space = format == RHI_Format::R10G10B10A2_Unorm ? VK_COLOR_SPACE_HDR10_ST2084_EXT : color_space; // HDR

            return color_space;
        }

        void set_hdr_metadata(const VkSwapchainKHR& swapchain)
        {
            VkHdrMetadataEXT hdr_metadata = {};
            hdr_metadata.sType = VK_STRUCTURE_TYPE_HDR_METADATA_EXT;
            hdr_metadata.displayPrimaryRed.x = 0.708f;
            hdr_metadata.displayPrimaryRed.y = 0.292f;
            hdr_metadata.displayPrimaryGreen.x = 0.170f;
            hdr_metadata.displayPrimaryGreen.y = 0.797f;
            hdr_metadata.displayPrimaryBlue.x = 0.131f;
            hdr_metadata.displayPrimaryBlue.y = 0.046f;
            hdr_metadata.whitePoint.x = 0.3127f;
            hdr_metadata.whitePoint.y = 0.3290f;
            const float nits_to_lumin = 10000.0f;
            //hdr_metadata.maxLuminance = Display::GetLuminanceMax() * nits_to_lumin;
            hdr_metadata.minLuminance = 0.001f * nits_to_lumin;
            hdr_metadata.maxContentLightLevel = 2000.0f;
            hdr_metadata.maxFrameAverageLightLevel = 500.0f;

            PFN_vkSetHdrMetadataEXT pfnVkSetHdrMetadataEXT = (PFN_vkSetHdrMetadataEXT)vkGetDeviceProcAddr(RHI_Context::device, "vkSetHdrMetadataEXT");
            LC_ASSERT(pfnVkSetHdrMetadataEXT != nullptr);
            pfnVkSetHdrMetadataEXT(RHI_Context::device, 1, &swapchain, &hdr_metadata);
        }

        VkSurfaceCapabilitiesKHR get_surface_capabilities(const VkSurfaceKHR surface)
        {
            VkSurfaceCapabilitiesKHR surface_capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(RHI_Context::device_physical, surface, &surface_capabilities);
            return surface_capabilities;
        }

        vector<VkPresentModeKHR> get_supported_present_modes(const VkSurfaceKHR surface)
        {
            uint32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(RHI_Context::device_physical, surface, &present_mode_count, nullptr);

            vector<VkPresentModeKHR> surface_present_modes(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(RHI_Context::device_physical, surface, &present_mode_count, &surface_present_modes[0]);
            return surface_present_modes;
        }

        VkPresentModeKHR get_present_mode(const VkSurfaceKHR surface, const RHI_Present_Mode present_mode)
        {
            // convert RHI_Present_Mode to VkPresentModeKHR
            VkPresentModeKHR vk_present_mode = VK_PRESENT_MODE_FIFO_KHR;
            if (present_mode == RHI_Present_Mode::Immediate)
            {
                vk_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
            else if (present_mode == RHI_Present_Mode::Mailbox)
            {
                vk_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
            }

            // return the present mode as is if the surface supports it
            vector<VkPresentModeKHR> surface_present_modes = get_supported_present_modes(surface);
            for (const VkPresentModeKHR supported_present_mode : surface_present_modes)
            {
                if (vk_present_mode == supported_present_mode)
                {
                    return vk_present_mode;
                }
            }

            // At this point we call back to VK_PRESENT_MODE_FIFO_KHR, which as per spec is always present
            DEBUG_LOG_WARN("Requested present mode is not supported. Falling back to VK_PRESENT_MODE_FIFO_KHR");
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        vector<VkSurfaceFormatKHR> get_supported_surface_formats(const VkSurfaceKHR surface)
        {
            uint32_t format_count;
            LC_VK_ASSERT_MSG(vkGetPhysicalDeviceSurfaceFormatsKHR(RHI_Context::device_physical, surface, &format_count, nullptr),
                "Failed to get physical device surface format count");

            vector<VkSurfaceFormatKHR> surface_formats(format_count);
            LC_VK_ASSERT_MSG(vkGetPhysicalDeviceSurfaceFormatsKHR(RHI_Context::device_physical, surface, &format_count, &surface_formats[0]),
                "Failed to get physical device surfaces");

            return surface_formats;
        }

        bool is_format_and_color_space_supported(const VkSurfaceKHR surface, RHI_Format* format, VkColorSpaceKHR color_space)
        {
            // Get supported surface formats
            vector<VkSurfaceFormatKHR> supported_formats = get_supported_surface_formats(surface);

            // NV supports RHI_Format::B8R8G8A8_Unorm instead of RHI_Format::R8G8B8A8_Unorm.
            if ((*format) == RHI_Format::R8G8B8A8_Unorm && RHI_Device::GetPrimaryPhysicalDevice()->IsNvidia())
            {
                (*format) = RHI_Format::B8R8G8A8_Unorm;
            }

            for (const VkSurfaceFormatKHR& supported_format : supported_formats)
            {
                bool support_format = supported_format.format == vulkan_format[rhi_format_to_index(*format)];
                bool support_color_space = supported_format.colorSpace == color_space;

                if (support_format && support_color_space)
                    return true;
            }

            return false;
        }

        VkCompositeAlphaFlagBitsKHR get_supported_composite_alpha_format(const VkSurfaceKHR surface)
        {
            vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags =
            {
                VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
            };

            // Get physical device surface capabilities
            VkSurfaceCapabilitiesKHR surface_capabilities;
            LC_VK_ASSERT_MSG(
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(RHI_Context::device_physical, surface, &surface_capabilities),
                "Failed to get surface capabilities");

            // Simply select the first composite alpha format available
            for (VkCompositeAlphaFlagBitsKHR& composite_alpha : composite_alpha_flags)
            {
                if (surface_capabilities.supportedCompositeAlpha & composite_alpha)
                {
                    return composite_alpha;
                };
            }

            return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        }
    }

    RHI_SwapChain::RHI_SwapChain(
        void* sdl_window,
        const uint32_t width,
        const uint32_t height,
        const RHI_Present_Mode present_mode,
        const uint32_t buffer_count,
        const bool hdr,
        const char* name
    )
    {
        LC_ASSERT_MSG(RHI_Device::IsValidResolution(width, height), "Invalid resolution");

        // Copy parameters
        m_format = hdr ? format_hdr : format_sdr;
        m_buffer_count = buffer_count;
        m_width        = width;
        m_height       = height;
        m_glfw_window   = sdl_window;
        m_object_name  = name;
        m_present_mode = present_mode;

        Create();
        AcquireNextImage();

       /* LC_SUBSCRIBE_TO_EVENT(EventType::WindowResized, LC_EVENT_HANDLER(ResizeToWindowSize));
        LC_SUBSCRIBE_TO_EVENT(EventType::WindowFullscreen, LC_EVENT_HANDLER(ResizeToWindowSize));*/

        ApplicationBase::Instance()->window->ResizeEvent.AddListener(std::bind(&RHI_SwapChain::OnResize, this, std::placeholders::_1, std::placeholders::_2));
    }

    RHI_SwapChain::~RHI_SwapChain()
    {
        Destroy();
    }

    void RHI_SwapChain::Create()
    {
        LC_ASSERT(m_glfw_window != nullptr);

        // Create surface
        VkSurfaceKHR surface = nullptr;
        {
           /* LC_ASSERT_MSG(
                SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(m_glfw_window), RHI_Context::instance, &surface),
                "Failed to created window surface");*/

            /*if (glfwCreateWindowSurface(RHI_Context::instance, static_cast<GLFWwindow*>(m_glfw_window), nullptr, &surface) != VK_SUCCESS)
            {
                DEBUG_LOG_ERROR("glfwCreateWindowSurface failed!");
            }*/

            // 用win32构建 todo: 如果跨平台需要修改
         /*   VkWin32SurfaceCreateInfoKHR createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_KHR;
            createInfo.hwnd = glfwGetWin32Window(static_cast<GLFWwindow*>(m_glfw_window));
            createInfo.hinstance = GetModuleHandle(nullptr);*/
            if (glfwCreateWindowSurface(RHI_Context::instance, static_cast<GLFWwindow*>(m_glfw_window), nullptr, &surface) != VK_SUCCESS)
            {
                DEBUG_LOG_ERROR("glfwCreateWindowSurface failed!");
            }

            VkBool32 present_support = false;
            LC_VK_ASSERT_MSG(vkGetPhysicalDeviceSurfaceSupportKHR(
                RHI_Context::device_physical,
                RHI_Device::QueueGetIndex(RHI_Queue_Type::Graphics),
                surface,
                &present_support),
                "Failed to get physical device surface support");

            LC_ASSERT_MSG(present_support, "The device does not support this kind of surface");
        }

        // Get surface capabilities
        VkSurfaceCapabilitiesKHR capabilities = get_surface_capabilities(surface);

        // Ensure that the surface supports the requested format and color space
        VkColorSpaceKHR color_space = get_color_space(m_format);
        LC_ASSERT_MSG(is_format_and_color_space_supported(surface, &m_format, color_space), "The surface doesn't support the requested format");

        // Clamp size between the supported min and max
        m_width  = Math::Helper::Clamp(m_width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width);
        m_height = Math::Helper::Clamp(m_height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        // Swap chain
        VkSwapchainKHR swap_chain;
        {
            VkSwapchainCreateInfoKHR create_info  = {};
            create_info.sType                     = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            create_info.surface                   = surface;
            create_info.minImageCount             = m_buffer_count;
            create_info.imageFormat               = vulkan_format[rhi_format_to_index(m_format)];
            create_info.imageColorSpace           = color_space;
            create_info.imageExtent               = { m_width, m_height };
            create_info.imageArrayLayers          = 1;
            create_info.imageUsage                = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // fer rendering on it
            create_info.imageUsage               |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;     // for blitting to it

            uint32_t queueFamilyIndices[] = { RHI_Device::QueueGetIndex(RHI_Queue_Type::Compute), RHI_Device::QueueGetIndex(RHI_Queue_Type::Graphics) };
            if (queueFamilyIndices[0] != queueFamilyIndices[1])
            {
                create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                create_info.queueFamilyIndexCount = 2;
                create_info.pQueueFamilyIndices   = queueFamilyIndices;
            }
            else
            {
                create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                create_info.queueFamilyIndexCount = 0;
                create_info.pQueueFamilyIndices   = nullptr;
            }

            create_info.preTransform   = capabilities.currentTransform;
            create_info.compositeAlpha = get_supported_composite_alpha_format(surface);
            create_info.presentMode    = get_present_mode(surface, m_present_mode);
            create_info.clipped        = VK_TRUE;
            create_info.oldSwapchain   = nullptr;

            LC_VK_ASSERT_MSG(vkCreateSwapchainKHR(RHI_Context::device, &create_info, nullptr, &swap_chain),
                "Failed to create swapchain");


            // todo: set hdr
            // set_hdr_metadata(swap_chain);
        }

        // Images
        {
            uint32_t image_count = 0;
            LC_VK_ASSERT_MSG(vkGetSwapchainImagesKHR(RHI_Context::device, swap_chain, &image_count, nullptr), "Failed to get swapchain image count");
            LC_VK_ASSERT_MSG(vkGetSwapchainImagesKHR(RHI_Context::device, swap_chain, &image_count, reinterpret_cast<VkImage*>(m_rhi_rt.data())), "Failed to get swapchain image count");

        	// transition layouts to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            if (RHI_CommandList* cmd_list = RHI_Device::CmdImmediateBegin(RHI_Queue_Type::Graphics))
            {
                for (uint32_t i = 0; i < m_buffer_count; i++)
                {
                    cmd_list->InsertBarrierTexture(
                        m_rhi_rt[i],
                        VK_IMAGE_ASPECT_COLOR_BIT,
                        0,
                        1,
                        1,
                        RHI_Image_Layout::Max,
                        RHI_Image_Layout::Attachment,
                        false
                    );

                    m_layouts[i] = RHI_Image_Layout::Attachment;
                }

                // end/flush
                RHI_Device::CmdImmediateSubmit(cmd_list);
            }
        }

        // image views
        {
            for (uint32_t i = 0; i < m_buffer_count; i++)
            {
                RHI_Device::SetResourceName(m_rhi_rt[i], RHI_Resource_Type::Texture, string(string("swapchain_image_") + to_string(i)));

                VkImageViewCreateInfo create_info           = {};
                create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                create_info.image                           = static_cast<VkImage>(m_rhi_rt[i]);
                create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                create_info.format                          = vulkan_format[rhi_format_to_index(m_format)];
                create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                create_info.subresourceRange.baseMipLevel   = 0;
                create_info.subresourceRange.levelCount     = 1;
                create_info.subresourceRange.baseArrayLayer = 0;
                create_info.subresourceRange.layerCount     = 1;
                create_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                create_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                create_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                create_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

                LC_ASSERT_MSG(vkCreateImageView(RHI_Context::device, &create_info, nullptr, reinterpret_cast<VkImageView*>(&m_rhi_rtv[i])) == VK_SUCCESS, "Failed to create swapchain RTV");
            }
        }

        m_rhi_surface   = static_cast<void*>(surface);
        m_rhi_swapchain = static_cast<void*>(swap_chain);

        // Semaphores
        for (uint32_t i = 0; i < m_buffer_count; i++)
        {
            string name = (string("swapchain_image_acquired_") + to_string(i));
            m_image_acquired_semaphore[i] = make_shared<RHI_Semaphore>(false, name.c_str());
            m_image_acquired_fence[i] = make_shared<RHI_Fence>(name.c_str());
        }
    }

    void RHI_SwapChain::Destroy()
    {
        for (void* image_view : m_rhi_rtv)
        {
            if (image_view)
            {
                RHI_Device::DeletionQueueAdd(RHI_Resource_Type::TextureView, image_view);
            }
        }

        m_rhi_rtv.fill(nullptr);
        m_image_acquired_semaphore.fill(nullptr);

        RHI_Device::QueueWaitAll();

        vkDestroySwapchainKHR(RHI_Context::device, static_cast<VkSwapchainKHR>(m_rhi_swapchain), nullptr);
        m_rhi_swapchain = nullptr;

        vkDestroySurfaceKHR(RHI_Context::instance, static_cast<VkSurfaceKHR>(m_rhi_surface), nullptr);
        m_rhi_surface = nullptr;
    }

    void RHI_SwapChain::Resize(const uint32_t width, const uint32_t height, const bool force /*= false*/)
    {
        LC_ASSERT(RHI_Device::IsValidResolution(width, height));

        if(width==0 || height==0)
        {
            return;
        }

        // Only resize if needed
        if (!force)
        {
            if (m_width == width && m_height == height)
                return;
        }

        // Save new dimensions
        m_width  = width;
        m_height = height;

        // Reset image index
        m_image_index          = numeric_limits<uint32_t>::max();
    	m_sync_index = std::numeric_limits<uint32_t>::max();

        Destroy();
        Create();
        AcquireNextImage();

        DEBUG_LOG_INFO("Resolution has been set to {}x{}", width, height);
    }

    void RHI_SwapChain::ResizeToWindowSize()
    {
        // todo swapChain需要Window句柄
        const auto& size = ApplicationBase::Instance()->window->GetSize();
        Resize(size.first, size.second);
    }

    void RHI_SwapChain::AcquireNextImage()
    {
        if (m_sync_index != numeric_limits<uint32_t>::max())
        {
            m_image_acquired_fence[m_sync_index]->Wait();
            m_image_acquired_fence[m_sync_index]->Reset();
        }

        // get sync objects
        m_sync_index = (m_sync_index + 1) % m_buffer_count;
        RHI_Semaphore* signal_semaphore = m_image_acquired_semaphore[m_sync_index].get();
        RHI_Fence* signal_fence = m_image_acquired_fence[m_sync_index].get();
        LC_ASSERT_MSG(signal_semaphore->GetStateCpu() != RHI_Sync_State::Submitted, "The semaphore is already signaled");

        // acquire next image
        LC_VK_ASSERT_MSG(vkAcquireNextImageKHR(
            RHI_Context::device,                                          // device
            static_cast<VkSwapchainKHR>(m_rhi_swapchain),                 // swapchain
            numeric_limits<uint64_t>::max(),                              // timeout - wait/block
            static_cast<VkSemaphore>(signal_semaphore->GetRhiResource()), // signal semaphore
            static_cast<VkFence>(signal_fence->GetRhiResource()),         // signal fence
            &m_image_index                                                // pImageIndex
        ), "Failed to acquire next image");

        // update sync state
        signal_semaphore->SetStateCpu(RHI_Sync_State::Submitted);
    }

    void RHI_SwapChain::OnResize(uint32_t width, uint32_t height)
    {
        Resize(width, height);
    }
    void RHI_SwapChain::Present()
    {
        // LC_ASSERT_MSG(!(SDL_GetWindowFlags(static_cast<SDL_Window*>(m_glfw_window)) & SDL_WINDOW_MINIMIZED), "Present should not be called for a minimized window");
        LC_ASSERT_MSG(m_rhi_swapchain != nullptr,                                                           "Invalid swapchain");
        LC_ASSERT_MSG(m_layouts[m_image_index] == RHI_Image_Layout::Present_Source,                            "Invalid layout");

        // Get the semaphores that present should wait for
        m_wait_semaphores.clear();
        {
            // Semaphores which are signaled when command lists have finished executing
            for (const shared_ptr<RHI_CommandPool>& cmd_pool : RHI_Device::GetCommandPools())
            {
                // The editor supports multiple windows, so we can be dealing with multiple swapchains
                if (m_object_id == cmd_pool->GetSwapchainId())
                {
                    RHI_Semaphore* semaphore_cmd_list = cmd_pool->GetCurrentCommandList()->GetSemaphoreProccessed();
                    if (semaphore_cmd_list->GetStateCpu() == RHI_Sync_State::Submitted)
                    {
                        m_wait_semaphores.emplace_back(semaphore_cmd_list);
                    }
                }
            }
            LC_ASSERT_MSG(!m_wait_semaphores.empty(), "Present() present should not be called if no work is to be presented");

            // Semaphore that's signaled when the image is acquired
            RHI_Semaphore* semaphore_image_aquired = m_image_acquired_semaphore[m_sync_index].get();
            LC_ASSERT(semaphore_image_aquired->GetStateCpu() == RHI_Sync_State::Submitted);
            m_wait_semaphores.emplace_back(semaphore_image_aquired);
        }

        RHI_Device::QueuePresent(m_rhi_swapchain, &m_image_index, m_wait_semaphores);
        AcquireNextImage();
    }

    void RHI_SwapChain::SetLayout(const RHI_Image_Layout& layout, RHI_CommandList* cmd_list)
    {
        if (m_layouts[m_image_index] == layout)
            return;

        cmd_list->InsertBarrierTexture(
            m_rhi_rt[m_image_index],
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 1,
            m_layouts[m_image_index],
            layout,
            false
        );

        m_layouts[m_image_index] = layout;
    }

    void RHI_SwapChain::SetHdr(const bool enabled)
    {
       /* if (enabled)
        {
            LC_ASSERT_MSG(Display::GetHdr(), "This display doesn't support HDR");
        }*/

        RHI_Format new_format = enabled ? format_hdr : format_sdr;

        if (new_format != m_format)
        {
            m_format = new_format;
            Resize(m_width, m_height, true);
            DEBUG_LOG_INFO("HDR has been {}", enabled ? "enabled" : "disabled");
        }
    }

    void RHI_SwapChain::SetVsync(const bool enabled)
    {
        // For v-sync, we could Mailbox for lower latency, but Fifo is always supported, so we'll assume that

        if ((m_present_mode == RHI_Present_Mode::Fifo) != enabled)
        {
            m_present_mode = enabled ? RHI_Present_Mode::Fifo : RHI_Present_Mode::Immediate;
            Resize(m_width, m_height, true);
            //Timer::OnVsyncToggled(enabled);// TODO:change
            DEBUG_LOG_INFO("VSync has been {}", enabled ? "enabled" : "disabled");
        }
    }

    bool RHI_SwapChain::GetVsync()
    {
        // For v-sync, we could Mailbox for lower latency, but Fifo is always supported, so we'll assume that
        return m_present_mode == RHI_Present_Mode::Fifo;
    }

    RHI_Image_Layout RHI_SwapChain::GetLayout() const
    {
        return m_layouts[m_image_index];
    }
}

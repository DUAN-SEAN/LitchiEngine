
#pragma once

//= INCLUDES =================
#include <memory>
#include <vector>
#include <array>
#include "RHI_Definitions.h"
//============================

namespace Spartan
{
    static const uint8_t max_buffer_count = 3;
    static const RHI_Format format_sdr    = RHI_Format::R8G8B8A8_Unorm;
    static const RHI_Format format_hdr    = RHI_Format::R10G10B10A2_Unorm;

    class SP_CLASS RHI_SwapChain : public RHI_Object
    {
    public:
        RHI_SwapChain() = default;
        RHI_SwapChain(
            void* sdl_window,
            const uint32_t width,
            const uint32_t height,
            const RHI_Present_Mode present_mode,
            const uint32_t buffer_count,
            const char* name
        );
        ~RHI_SwapChain();

        // Size
        void Resize(uint32_t width, uint32_t height, const bool force = false);
        void ResizeToWindowSize();

        // HDR
        void SetHdr(const bool enabled);
        bool IsHdr() const { return m_format == format_hdr; }

        // VSync
        void SetVsync(const bool enabled);
        bool GetVsync();

        void Present();

        // Properties
        uint32_t GetWidth()       const { return m_width; }
        uint32_t GetHeight()      const { return m_height; }
        uint32_t GetBufferCount() const { return m_buffer_count; }
        RHI_Format GetFormat()    const { return m_format; }
        void* GetRhiRt()          const { return m_rhi_rt[m_image_index]; }
        void* GetRhiRtv()         const { return m_rhi_rtv[m_image_index]; }

        // Layout
        RHI_Image_Layout GetLayout() const;
        void SetLayout(const RHI_Image_Layout& layout, RHI_CommandList* cmd_list);

    private:
        void Create();
        void Destroy();
        void AcquireNextImage();

        // Main
        bool m_windowed                 = false;
        uint32_t m_buffer_count         = 0;
        uint32_t m_width                = 0;
        uint32_t m_height               = 0;
        RHI_Format m_format             = RHI_Format::Undefined;
        RHI_Present_Mode m_present_mode = RHI_Present_Mode::Immediate;

        // Misc
        uint32_t m_sync_index                                    = std::numeric_limits<uint32_t>::max();
        uint32_t m_image_index                                   = std::numeric_limits<uint32_t>::max();
        uint32_t m_image_index_previous                          = m_image_index;
        void* m_sdl_window                                       = nullptr;
        std::array<RHI_Image_Layout, max_buffer_count> m_layouts = { RHI_Image_Layout::Undefined, RHI_Image_Layout::Undefined, RHI_Image_Layout::Undefined };
        std::array<std::shared_ptr<RHI_Semaphore>, max_buffer_count> m_acquire_semaphore;
        std::vector<RHI_Semaphore*> m_wait_semaphores;

        // RHI
        void* m_rhi_swapchain                          = nullptr;
        void* m_rhi_surface                            = nullptr;
        std::array<void*, max_buffer_count> m_rhi_rt  = { nullptr, nullptr, nullptr};
        std::array<void*, max_buffer_count> m_rhi_rtv = { nullptr, nullptr, nullptr};
    };
}

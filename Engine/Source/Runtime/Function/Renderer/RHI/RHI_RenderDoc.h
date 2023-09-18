
#pragma once

//= INCLUDES ===========
#include "Runtime/Core/Definitions.h"
//======================

namespace Spartan
{
    class SP_CLASS RHI_RenderDoc
    {
    public:
        static void OnPreDeviceCreation();
        static void Shutdown();
        static void FrameCapture();
        static void StartCapture();
        static void EndCapture();
        static void LaunchRenderDocUi();
        static bool IsEnabled();
    };
}

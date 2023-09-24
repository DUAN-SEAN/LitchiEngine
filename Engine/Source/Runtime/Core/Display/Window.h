
#pragma once

//= INCLUDES ===========
#include <cstdint>

#include "Runtime/Core/Definitions.h"
//======================

namespace Spartan
{
    class SP_CLASS Window
    {
    public:
        static void Initialize();
        static void Shutdown();
        static void Tick();

        // Behaviour
        static void Show();
        static void Hide();
        static void Focus();
        static void FullScreen();
        static void Windowed();
        static void ToggleFullScreen();
        static void FullScreenBorderless();
        static void Minimise();
        static void Maximise();

        // Size
        static void SetSize(const uint32_t width, const uint32_t height);
        static uint32_t GetWidth();
        static uint32_t GetHeight();
        static float GetDpiScale();

        // Misc
        static void* GetHandleSDL();
        static void* GetHandleRaw();
        static bool WantsToClose();
        static bool IsMinimised();
        static bool IsFullScreen();

    private:
        static void CreateAndShowSplashScreen();
        static void OnFirstFrameCompleted();
    };
}

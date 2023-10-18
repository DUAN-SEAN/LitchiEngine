
#pragma once

//= INCLUDES ======================
#include <string>
#include "Runtime/Core/Definitions.h"
//=================================

namespace LitchiRuntime
{
    class Font;

    class SP_CLASS FontImporter
    {
    public:
        static void Initialize();
        static void Shutdown();
        static bool LoadFromFile(Font* font, const std::string& file_path);
    };
}

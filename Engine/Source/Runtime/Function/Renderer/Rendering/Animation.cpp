

//= INCLUDES =========
#include "Runtime/Core/pch.h"
#include "Animation.h"
//====================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    Animation::Animation(): IResource(ResourceType::Animation)
    {

    }

    bool Animation::LoadFromFile(const string& filePath)
    {
        return true;
    }

    bool Animation::SaveToFile(const string& filePath)
    {
        return true;
    }
}

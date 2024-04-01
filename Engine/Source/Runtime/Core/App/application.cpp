
#include "Runtime/Core/pch.h"
#include "application.h"
#include "ApplicationBase.h"
namespace LitchiRuntime
{
    ApplicationBase* Application::s_instance = nullptr;

    void Application::Initialize(ApplicationBase* instance) {
        s_instance = instance;
        if(!s_instance->Initialize())
        {
            throw std::runtime_error("Failed to Initialization Application");
        }
    }

    void Application::Run() {
        s_instance->Run();
    }
}
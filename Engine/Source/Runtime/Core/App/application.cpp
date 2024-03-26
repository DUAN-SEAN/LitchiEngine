
#include "Runtime/Core/pch.h"
#include "application.h"
#include "ApplicationBase.h"
namespace LitchiRuntime
{
    ApplicationBase* Application::s_instance = nullptr;

    void Application::Initiliaze(ApplicationBase* instance) {
        s_instance = instance;
        if(!s_instance->Init())
        {
            throw std::runtime_error("Failed to Init Application");
        }
    }

    const std::string& Application::GetDataPath() {
        return s_instance->GetProjectPath();
    }

    void Application::Run() {
        s_instance->Run();
    }
}
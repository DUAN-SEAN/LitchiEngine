
#include "application.h"
#include "ApplicationBase.h"
namespace LitchiRuntime
{
    ApplicationBase* Application::s_instance = nullptr;

    void Application::Initiliaze(ApplicationBase* instance) {
        s_instance = instance;
        s_instance->Init();
    }

    const std::string& Application::GetDataPath() {
        return s_instance->GetProjectPath();
    }

    void Application::Run() {
        s_instance->Run();
    }
}
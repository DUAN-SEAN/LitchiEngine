
#pragma once

#include <string>

namespace LitchiRuntime
{
    class ApplicationBase;
    class Application {
    public:
        static void Initiliaze(ApplicationBase* instance);

        static const std::string& GetDataPath();

        static void Run();
    private:
        static ApplicationBase* s_instance;
    };
}

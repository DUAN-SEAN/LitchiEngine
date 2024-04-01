
#pragma once

namespace LitchiRuntime
{
    class ApplicationBase;
    class Application {
    public:

        static void Initialize(ApplicationBase* instance);

        static void Run();

    private:
        static ApplicationBase* s_instance;
    };
}

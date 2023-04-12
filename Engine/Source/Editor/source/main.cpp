#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include "Runtime/Core/App/application.h"
#include "Runtime/Platform/App/application_standalone.h"

int main(int argc, char** argv)
{
    // std::filesystem::path executable_path(argv[0]);

    Application application;
    application.Init(new ApplicationStandalone());

    return 0;
}

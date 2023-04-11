#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include "Runtime/Core/App/application.h"

int main(int argc, char** argv)
{
    // std::filesystem::path executable_path(argv[0]);

    Application application;
    application.Init(nullptr);

    return 0;
}

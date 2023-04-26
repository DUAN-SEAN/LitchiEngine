#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Editor/include/application_editor.h"
#include "Editor/include/login_scene.h"
#include "Runtime/Core/App/application.h"
#include "Runtime/Platform/App/application_standalone.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

using namespace LitchiRuntime;
using namespace LitchiEditor;
int main(int argc, char** argv)
{
    // std::filesystem::path executable_path(argv[0]);

    Application application;
    // auto application_standalone=new ApplicationStandalone();
    auto application_standalone=new ApplicationEditor();

    char* projectPath = nullptr;
    projectPath = _getcwd(nullptr, 1);
    std::string filePath(projectPath);

    application_standalone->SetDataPath(filePath+"/../../Assets/");

    delete projectPath;

    application.Initiliaze(application_standalone);
    auto s =  application.GetDataPath();

    DEBUG_LOG_INFO( s);
    
    GameObject* o = new GameObject("LoginScene");
    auto loginScene = o->AddComponent<LoginScene>();

    application.Run();

    delete o;
    delete application_standalone;

    return 0;
}

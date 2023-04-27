#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Editor/include/application_editor.h"
#include "Editor/include/login_scene.h"
#include "Runtime/Core/App/application.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Platform/App/application_standalone.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Scene/scene_manager.h"

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

    auto scene= SceneManager::CreateScene("DefaultScene");

    GameObject* o = new GameObject("LoginScene", scene);
    auto loginScene = o->AddComponent<LoginScene>();

    auto json =  SerializerManager::SerializeToJson(scene);
    DEBUG_LOG_INFO(json);


    application.Run();

    delete o;
    delete application_standalone;

    return 0;
}

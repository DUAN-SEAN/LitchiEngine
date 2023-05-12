#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Runtime/Core/App/application.h"
#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

using namespace LitchiEditor;
using namespace LitchiRuntime;
int main(int argc, char** argv)
{
    /*auto vecType  = type::get<glm::vec3>();
    glm::vec3 value(0.0, 0.0, 0.0);
    glm::vec3& valueRef = value;
    auto property = vecType.get_property("x");
    instance ins = valueRef;
    auto var =  property.get_value(ins);
    property.set_value(ins, 3.0f);
    auto x = valueRef.x;
    auto x2 = value.x;*/

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


    //auto s =  application.GetDataPath();

    //DEBUG_LOG_INFO( s);

    //auto scene= SceneManager::CreateScene("DefaultScene");

    //GameObject* o = new GameObject("LoginScene", scene);
    //auto loginScene = o->AddComponent<LoginScene>();

    //auto json =  SerializerManager::SerializeToJson(scene);
    //DEBUG_LOG_INFO(json);


    application.Run();

    // delete o;
    delete application_standalone;

    return 0;
}

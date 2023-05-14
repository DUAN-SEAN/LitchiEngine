#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Runtime/Core/App/application.h"
#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Meta/Reflection/propery_field.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

using namespace LitchiEditor;
using namespace LitchiRuntime;
int main(int argc, char** argv)
{
    std::vector<std::string> propertyNameList;
    propertyNameList.push_back("position");
    propertyNameList.push_back("x");

    auto vecType  = type::get<glm::vec3>();

    // 创建transform
    auto transformType = type::get<Transform>();
    auto tranVar = transformType.create();

    // 设置pos
    auto& tran = *tranVar.get_value<Transform*>();
    tran.set_position(glm::vec3(5.0, 0.0, 0.0));


    PropertyField property_field(&tran, propertyNameList);
    property_field.SetValue(12.0);



    // 获取pos的熟悉
    auto posProp = transformType.get_property("position");
    bool isValid = posProp.is_valid();

    // 获取vec3
    auto vec = posProp.get_value(tranVar).get_value<glm::vec3>();
    posProp.set_value(tranVar, glm::vec3(3.0, 0.0, 0.0));
    vec = posProp.get_value(tranVar).get_value<glm::vec3>();
    auto& vecVar = posProp.get_value(tranVar);

    // 获取x属性
    auto propertyX = posProp.get_type().get_property("x");
	isValid = propertyX.is_valid();

    // 获取x值
    auto xVar = propertyX.get_value(vecVar);
    float xV = xVar.get_value<float>();

    // 设置x的值
    variant setValue = 6.0f;
    isValid = setValue.convert(propertyX.get_type());
    isValid = propertyX.set_value(vecVar, setValue);
    
    // 获取x的值
	xV = propertyX.get_value(vecVar).to_float();
    posProp.set_value(tranVar, vecVar);

    auto result = tran.position();

    glm::vec3 value(0.0, 0.0, 0.0);
    glm::vec3& valueRef = value;
    auto property = vecType.get_property("x");
    instance ins = valueRef;
    auto var =  property.get_value(ins);
    property.set_value(ins, 3.0f);
    auto x = valueRef.x;
    auto x2 = value.x;

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

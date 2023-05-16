#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Runtime/Core/App/application.h"
#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Meta/Reflection/propery_field.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Renderer/Resources/Loaders/ModelLoader.h"
#include "Runtime/Function/Renderer/Resources/Loaders/TextureLoader.h"

using namespace LitchiEditor;
using namespace LitchiRuntime;
int main(int argc, char** argv)
{
	/*Loaders::TextureLoader::Create("C:/Users/lenovo/Desktop/1.png", ETextureFilteringMode::LINEAR, ETextureFilteringMode::LINEAR, true);
	Loaders::ModelLoader::Create("");*/

	// 测试代码
	std::vector<std::string> propertyNameList;
	propertyNameList.push_back("position");
	propertyNameList.push_back("x");

	auto vecType = type::get<glm::vec3>();

	// 创建transform
	auto transformType = type::get<Transform>();
	auto tranVar = transformType.create();

	// 设置pos
	auto& tran = *tranVar.get_value<Transform*>();
	tran.set_position(glm::vec3(5.0, 0.0, 0.0));

	// 创建propertyField
	PropertyField property_field(&tran, propertyNameList);
	property_field.SetValue(12.0);

	Application application;
	// auto application_standalone=new ApplicationStandalone();
	auto application_standalone = new ApplicationEditor();

	char* projectPath = nullptr;
	projectPath = _getcwd(nullptr, 1);
	std::string filePath(projectPath);

	application_standalone->SetDataPath(filePath + "/../../Assets/");

	delete projectPath;

	application.Initiliaze(application_standalone);

	application.Run();

	// delete o;
	delete application_standalone;

	return 0;
}

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

	LitchiRuntime::Resource::MaterialRes res;
	res.shaderPath = "XXXX";
	res.settings.backfaceCulling = true;

	auto uniformFloat = new Resource::UniformInfoFloat();
	uniformFloat->name = "shineness";
	uniformFloat->value = 64;

	auto uniformVec4 = new Resource::UniformInfoVector4();
	uniformVec4->name = "diffuseColor";
	uniformVec4->vector = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	auto uniformFloatType = uniformFloat->get_type();
	auto uniformVec4Type = uniformVec4->get_type();
	auto uniform = uniformFloat;
	auto uniformType = uniform->get_type();


	res.uniformInfoList.push_back(uniformFloat);
	res.uniformInfoList.push_back(uniformVec4);

	auto serializeJson = SerializerManager::SerializeToJson(res);

	LitchiRuntime::Resource::MaterialRes res2;
	SerializerManager::DeserializeFromJson(serializeJson, res2);

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

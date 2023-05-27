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

	auto uniformPath = new Resource::UniformInfoPath();
	uniformPath->name = "diffuseTexture";
	uniformPath->path = "";

	auto uniformPath2 = new Resource::UniformInfoPath();
	uniformPath2->name = "specularTexture";
	uniformPath2->path = "";

	auto unfiromBase = new Resource::UniformInfoBase();
	auto type1 = uniformFloat->get_type();
	auto type2 = unfiromBase->get_type();
	auto flag = type2.is_base_of(type1);

	rttr::instance ins = unfiromBase;
	auto insType= ins.get_type();
	auto isWrapper = insType.is_wrapper();
	auto is_pointer = insType.is_pointer();
	auto type22 = type2.get_raw_type();
	auto insType2 = insType.get_raw_type();
	auto vv = insType.get_metadata("Polymorphic");
	if(vv.is_valid())
	{
		auto f = vv.to_bool();
	}
	for (auto base_class : uniformFloat->get_type().get_base_classes())
	{
		auto v = base_class.get_metadata("Polymorphic");
	}

	auto uniformVec4 = new Resource::UniformInfoVector4();
	uniformVec4->name = "diffuseColor";
	uniformVec4->vector = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	auto uniformFloatType = uniformFloat->get_type();
	auto uniformVec4Type = uniformVec4->get_type();
	auto uniform = uniformFloat;
	auto uniformType = uniform->get_type();


	res.uniformInfoList.push_back(uniformFloat);
	res.uniformInfoList.push_back(uniformVec4);
	res.uniformInfoList.push_back(uniformPath);
	res.uniformInfoList.push_back(uniformPath2);

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

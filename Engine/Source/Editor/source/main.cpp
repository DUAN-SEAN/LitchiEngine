
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Runtime/Core/Definitions.h"
#include "Runtime/Core/App/application.h"
#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"

using namespace LitchiEditor;
using namespace LitchiRuntime;
int main(int argc, char** argv)
{
	Application application;
	auto application_standalone = new ApplicationEditor();

	//char* projectPath = nullptr;
	//projectPath = _getcwd(nullptr, 1);
	//std::string filePath(projectPath);
	// delete projectPath;

	application.Initiliaze(application_standalone);

	application.Run();

	// delete o;
	delete application_standalone;

	return 0;
}

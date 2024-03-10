#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Runtime/Core/App/application.h"
#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"

using namespace LitchiEditor;
using namespace LitchiRuntime;
int main(int argc, char** argv)
{
	Application application;
	// auto application_standalone=new ApplicationStandalone();
	auto application_standalone = new ApplicationEditor();

	char* projectPath = nullptr;
	projectPath = _getcwd(nullptr, 1);
	std::string filePath(projectPath);



#ifdef _DEBUG
	application_standalone->SetProjectPath(filePath + "\\Debug\\Project\\");
#else
	application_standalone->SetProjectPath(filePath + "\\Project\\");
#endif

	delete projectPath;

	application.Initiliaze(application_standalone);

	application.Run();

	// delete o;
	delete application_standalone;

	return 0;
}

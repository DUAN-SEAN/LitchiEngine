#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "Runtime/Core/App/application.h"
#include "Standalone/include/ApplicationStandalone.h"

using namespace LitchiRuntime;
int main(int argc, char** argv)
{
	static Application application;
	const auto application_standalone = new LitchiStandalone::ApplicationStandalone();

	const char* projectPath = _getcwd(nullptr, 1);
	std::string filePath(projectPath);

	// todo temp , must input in argv
#ifdef _DEBUG
	application_standalone->SetProjectPath(filePath + "\\..\\..\\..\\..\\..\\DemoProject\\ProjectTemplete\\");
#else
	application_standalone->SetProjectPath(filePath + "\\..\\..\\..\\..\\..\\DemoProject\\ProjectTemplete\\");
#endif
	delete projectPath;

	application.Initiliaze(application_standalone);
	application.Run();

	delete application_standalone;
	return 0;
}

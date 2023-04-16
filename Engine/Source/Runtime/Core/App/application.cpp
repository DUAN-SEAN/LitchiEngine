
#include "application.h"
#include "application_base.h"

ApplicationBase* Application::instance_= nullptr;

void Application::Initiliaze(ApplicationBase* instance) {
    instance_=instance;
    instance_->Init();
}

const std::string& Application::GetDataPath() {
    return instance_->data_path();
}

void Application::Run() {
    instance_->Run();
}
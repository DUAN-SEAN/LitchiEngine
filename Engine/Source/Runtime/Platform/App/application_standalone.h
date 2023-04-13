
#ifndef UNTITLED_APPLICATION_STANDALONE_H
#define UNTITLED_APPLICATION_STANDALONE_H

#include <string>
#include "Runtime/Core/App/application_base.h"

class GLFWwindow;
class ApplicationStandalone : public ApplicationBase{
public:
    ApplicationStandalone():ApplicationBase(){}
    ~ApplicationStandalone(){}

    void Init() override;

    void OneFrame() override;

    void Run();

public:
    /// 初始化图形库，例如glfw
    virtual void InitGraphicsLibraryFramework() override;

    virtual void Exit() override;
private:
    GLFWwindow* glfw_window_;
};


#endif //UNTITLED_APPLICATION_STANDALONE_H

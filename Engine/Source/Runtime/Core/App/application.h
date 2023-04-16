
#ifndef UNTITLED_APPLICATION_H
#define UNTITLED_APPLICATION_H

#include <string>

class ApplicationBase;
class Application {
public:
    static void Initiliaze(ApplicationBase* instance);

    static const std::string& GetDataPath();

    static void Run();
private:
    static ApplicationBase* instance_;
};


#endif //UNTITLED_APPLICATION_H

//
// Created by captain on 2021/7/10.
//

#ifndef UNTITLED_LOGINSCENE_H
#define UNTITLED_LOGINSCENE_H

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "glm.hpp"

using namespace std;
using namespace LitchiRuntime;

class Transform;
class Camera;
class LoginScene:public Component{
public:
    LoginScene();

    void Awake();

    void Update();

private:
    Transform* transform_fishsoup_pot_;
    Transform* transform_camera_1_;
    Camera*     camera_1_;
    Transform* transform_camera_2_;
    Camera*     camera_2_;
    glm::vec2 last_frame_mouse_position_;//上一帧的鼠标位置
};


#endif //UNTITLED_LOGINSCENE_H

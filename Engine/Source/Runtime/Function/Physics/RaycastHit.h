#pragma once

#include "glm.hpp"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
namespace LitchiRuntime
{
    /// 射线检测结果
    class RaycastHit {
    public:
        RaycastHit() {}
        ~RaycastHit() {}

        glm::vec3& position() { return position_; }
        void set_position(float x, float y, float z) {
            position_ = glm::vec3(x, y, z);
        }

        GameObject* game_object() { return game_object_; }
        void set_game_object(GameObject* game_object) { game_object_ = game_object; }
    private:
        glm::vec3 position_;//击中物体的坐标
        GameObject* game_object_;//击中的物体
    };
}

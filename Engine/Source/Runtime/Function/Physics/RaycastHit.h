#pragma once

#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Core/Math/Vector3.h"

namespace LitchiRuntime
{
    /// 射线检测结果
    class RaycastHit {
    public:
        RaycastHit() {}
        ~RaycastHit() {}

        Vector3& position() { return position_; }
        void set_position(float x, float y, float z) {
            position_ = Vector3(x, y, z);
        }

        GameObject* game_object() { return game_object_; }
        void set_game_object(GameObject* game_object) { game_object_ = game_object; }
    private:
        Vector3 position_;//击中物体的坐标
        GameObject* game_object_;//击中的物体
    };
}

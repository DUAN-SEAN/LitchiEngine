
#ifndef UNTITLED_TRANSFORM_H
#define UNTITLED_TRANSFORM_H

#include "glm.hpp"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/transform2.hpp>
#include "gtx/quaternion.hpp"

namespace LitchiRuntime
{
    class Transform : public Component {
    public:
        Transform();
        ~Transform();

        glm::vec3 position() const { return position_; }
        glm::quat rotation() const { return rotation_; }
        glm::vec3 scale() const { return scale_; }
        glm::mat4 toWorldMatrix() const
        {

            glm::mat4 trans = glm::translate(position_);
            glm::mat4 rotation = glm::toMat4(rotation_);
            glm::mat4 scale = glm::scale(scale_); //缩放;
            return trans * rotation * scale;
        }

        void set_position(glm::vec3 position) { position_ = position; }
        void set_rotation(glm::quat rotation) { rotation_ = rotation; }
        void set_scale(glm::vec3 scale) { scale_ = scale; }

    private:
        glm::vec3 position_;
        glm::quat rotation_;
        glm::vec3 scale_;

        RTTR_ENABLE(Component);
    };
}
#endif //UNTITLED_TRANSFORM_H

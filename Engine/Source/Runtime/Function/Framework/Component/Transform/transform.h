
#ifndef UNTITLED_TRANSFORM_H
#define UNTITLED_TRANSFORM_H

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Core/Math/FTransform.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>
#include "gtx/quaternion.hpp"

namespace LitchiRuntime
{
    class Transform : public Component {
    public:
        Transform();
        ~Transform();

        glm::vec3 position() const { return m_transform.GetLocalPosition(); }
        glm::quat rotation() const { return m_transform.GetLocalRotation(); }
        glm::vec3 scale() const { return m_transform.GetLocalScale(); }
        glm::mat4 toWorldMatrix() const
        {

            glm::mat4 trans = glm::translate(m_transform.GetWorldPosition());
            glm::mat4 rotation = glm::toMat4(m_transform.GetWorldRotation());
            glm::mat4 scale = glm::scale(m_transform.GetWorldScale()); //缩放;
            return trans * rotation * scale;
        }

        void set_position(glm::vec3 position) { m_transform.SetLocalPosition(position); }
        void set_rotation(glm::quat rotation) { m_transform.SetLocalRotation(rotation); }
        void set_scale(glm::vec3 scale) { m_transform.SetLocalScale(scale); }

    private:
        /*glm::vec3 position_;
        glm::quat rotation_;
        glm::vec3 scale_;*/
        FTransform m_transform;


        RTTR_ENABLE(Component);
    };
}
#endif //UNTITLED_TRANSFORM_H

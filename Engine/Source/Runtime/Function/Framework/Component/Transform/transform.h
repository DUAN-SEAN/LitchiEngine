
#pragma once

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

        glm::vec3 GetLocalPosition() const { return m_transform.GetLocalPosition(); }
        glm::quat GetLocalRotation() const { return m_transform.GetLocalRotation(); }
        glm::vec3 GetLocalScale() const { return m_transform.GetLocalScale(); }
        glm::mat4 GetWorldMatrix() const
        {

            glm::mat4 trans = glm::translate(m_transform.GetWorldPosition());
            glm::mat4 rotation = glm::toMat4(m_transform.GetWorldRotation());
            glm::mat4 scale = glm::scale(m_transform.GetWorldScale()); //缩放;
            return trans * rotation * scale;
        }

        void SetLocalPosition(glm::vec3 position) { m_transform.SetLocalPosition(position); }
        void SetLocalRotation(glm::quat rotation) { m_transform.SetLocalRotation(rotation); }
        void SetLocalScale(glm::vec3 scale) { m_transform.SetLocalScale(scale); }

        FTransform& GetTransform() { return m_transform; }

    private:

        FTransform m_transform;

        RTTR_ENABLE(Component)
    };
}

﻿
#ifndef UNTITLED_TRANSFORM_H
#define UNTITLED_TRANSFORM_H

#include "core/type.hpp"
#include "glm.hpp"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>

class Transform : public Component{
public:
    Transform();
    ~Transform();

    glm::vec3 position() const {return position_;}
    glm::quat rotation() const {return rotation_;}
    glm::vec3 scale() const {return scale_;}

    void set_position(glm::vec3 position){position_=position;}
    void set_rotation(glm::quat rotation){rotation_=rotation;}
    void set_scale(glm::vec3 scale){scale_=scale;}

private:
    glm::vec3 position_;
    glm::quat rotation_;
    glm::vec3 scale_;

RTTR_ENABLE();
};
#endif //UNTITLED_TRANSFORM_H
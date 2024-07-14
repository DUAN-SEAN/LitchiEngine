
#include "Runtime/Core/pch.h"
#include "CharacterController.h"

#include "rttr/registration"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    CharacterController::CharacterController() :Component(), m_radius(1.0f),m_height(2.0f)
    {

    }

    CharacterController::~CharacterController() {

    }

    void CharacterController::PostResourceModify()
    {
        Component::PostResourceLoaded();
    }

}
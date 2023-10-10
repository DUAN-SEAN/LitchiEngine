
#pragma once

//= INCLUDES =======
#include "Vector3.h"
//==================

namespace LitchiRuntime
{
    class Sphere
    {
    public:
        Sphere();
        Sphere(const Vector3& center, const float radius);
        ~Sphere() = default;

        Vector3 center;
        float radius;
    };
}

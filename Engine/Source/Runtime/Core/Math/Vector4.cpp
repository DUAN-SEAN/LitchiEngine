
//= INCLUDES =======
#include "../pch.h"
#include "Vector4.h"
//==================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    const Vector4 Vector4::One(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 Vector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::Infinity(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
    const Vector4 Vector4::InfinityNeg(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity());

    Vector4::Vector4(const Vector3& value, float w)
    {
        this->x = value.x;
        this->y = value.y;
        this->z = value.z;
        this->w = w;
    }

    Vector4::Vector4(const Vector3& value)
    {
        this->x = value.x;
        this->y = value.y;
        this->z = value.z;
        this->w = 0.0f;
    }

    string Vector4::ToString() const
    {
        char tempBuffer[200];
        sprintf(tempBuffer, "X:%f, Y:%f, Z:%f, W:%f", x, y, z, w);
        return string(tempBuffer);
    }
}


//= INCLUDES =======
#include "../pch.h"
//==================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
    const Vector3 Vector3::One(1.0f, 1.0f, 1.0f);
    const Vector3 Vector3::Left(-1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
    const Vector3 Vector3::Down(0.0f, -1.0f, 0.0f);
    const Vector3 Vector3::Forward(0.0f, 0.0f, 1.0f);
    const Vector3 Vector3::Backward(0.0f, 0.0f, -1.0f);
    const Vector3 Vector3::Infinity(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
    const Vector3 Vector3::InfinityNeg(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity());

    string Vector3::ToString() const
    {
        char buffer[200];
        sprintf(buffer, "X:%f, Y:%f, Z:%f", x, y, z);
        return string(buffer);
    }

    Vector3::Vector3(const Vector4& vector)
    {
        x = vector.x;
        y = vector.y;
        z = vector.z;
    }
}

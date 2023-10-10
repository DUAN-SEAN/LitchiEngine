
//= INCLUDES =======
#include "../pch.h"
//==================

//= NAMESPACES =====
using namespace std;
//==================

//= Based on ==========================================================================//
// http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/index.htm //
// Heading    -> Yaw        -> Y-axis                                                       //
// Attitude    -> Pitch    -> X-axis                                                       //
// Bank        -> Roll        -> Z-axis                                                       //
//=====================================================================================//

namespace LitchiRuntime
{
    const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

    //= FROM ==================================================================================
    void Quaternion::FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
    {
        *this = Matrix(
            xAxis.x, xAxis.y, xAxis.z, 0.0f,
            yAxis.x, yAxis.y, yAxis.z, 0.0f,
            zAxis.x, zAxis.y, zAxis.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        ).GetRotation();
    }
    //========================================================================================

    string Quaternion::ToString() const
    {
        char tempBuffer[200];
        sprintf(tempBuffer, "X:%f, Y:%f, Z:%f, W:%f", x, y, z, w);
        return string(tempBuffer);
    }
}

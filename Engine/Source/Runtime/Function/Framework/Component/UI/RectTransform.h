
#pragma once

#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Quaternion.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include <Runtime/Core/Math/Vector2.h>
namespace LitchiRuntime
{
    class UICanvas;
    class RectTransform : public Transform {
    public:
        RectTransform();
        ~RectTransform();

        void Awake() override;

        // Pos
        void SetPos(Vector3 pos) { m_pos = pos; }
        Vector3 GetPos() { return m_pos; }

        // Size
        void SetSize(Vector2 size) { m_size = size; }
        Vector2 GetSize() { return m_size; }

    private:

        // Canvas
        UICanvas* m_canvas;

        // transform
        Vector3 m_pos;
        Vector2 m_size;
        Matrix m_rectTransformMatrix;

    private:



        RTTR_ENABLE(Component)
    };
}

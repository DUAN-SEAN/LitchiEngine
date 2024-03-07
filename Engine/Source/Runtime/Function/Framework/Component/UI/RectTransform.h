
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

        void OnAwake() override;
        void OnUpdate() override;
        void OnEditorUpdate() override;

        // Pos
        void SetPos(Vector3 pos) { m_pos = pos; }
        Vector3 GetPos() { return m_pos; }

        // Size
        void SetSize(Vector2 size) { m_size = size; }
        Vector2 GetSize() { return m_size; }

    public:
        void PostResourceLoaded() override;
        void PostResourceModify() override;
    private:

        void UpdateRectTransform();
        void UpdateCanvas();

        // Canvas
        UICanvas* m_canvas = nullptr;

        // transform
        Vector3 m_pos;// ui resolution pos, if (0,0) is top-left 
        Vector2 m_size;// ui resolution size if(10,10) is 10 pixel width and height
        Matrix m_rectTransformMatrix;

    private:



        RTTR_ENABLE(Transform)
    };
}

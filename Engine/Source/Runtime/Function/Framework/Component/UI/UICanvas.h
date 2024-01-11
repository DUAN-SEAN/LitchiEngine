
#pragma once

#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Quaternion.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

namespace LitchiRuntime
{
    class UICanvas : public Component {
    public:
        UICanvas();
        ~UICanvas();

        void Awake() override;
        void Update() override;

        void SetResolution(Vector2 resolution) { m_resolution = resolution; }
        Vector2 GetResolution() { return m_resolution; }

    private:

        // Init
        void InitCanvasCamera();
        void InitCanvasTransform();

        Vector2 m_resolution;
        RenderCamera* m_canvasCamera;

    private:

        // FTransform m_transform;

        RTTR_ENABLE(Component)
    };
}

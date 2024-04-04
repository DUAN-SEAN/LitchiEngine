
#pragma once

#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Quaternion.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

namespace LitchiRuntime
{
    /**
     * @brief UICanvas Component
     * @note The Canvas component represents the abstract space in which the UI is laid out and rendered.
     *       All UI elements must be children of a GameObject that has a Canvas component attached. 
    */
    class UICanvas : public Component {
    public:

        /**
         * @brief Default Constructor
        */
        UICanvas();

        /**
         * @brief Default Destructor
        */
        ~UICanvas() override;

        /**
         * @brief Call before object resource loaded
         * when instantiate prefab, add component, resource loaded etc
         * after call resource load completed
        */
        void PostResourceLoaded() override;

        void SetResolution(Vector2 resolution) { m_resolution = resolution; }
        Vector2 GetResolution() { return m_resolution; }

    public:

        /**
         * @brief Called when the scene start right before OnStart
         * It allows you to apply prioritized game logic on scene start
        */
        void OnAwake() override;

        /**
         * @brief Called every frame
        */
        void OnUpdate() override;

    private:

        /**
         * @brief Init Canvas Camera
        */
        void InitCanvasCamera();

        /**
         * @brief Init Canvas Transform
        */
        void InitCanvasTransform();

        /**
         * @brief Canvas resolution
        */
        Vector2 m_resolution;

    private:

        // FTransform m_transform;

        RTTR_ENABLE(Component)
    };
}


#pragma once

#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Quaternion.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include <Runtime/Core/Math/Vector2.h>
namespace LitchiRuntime
{
    class UICanvas;

    /**
     * @brief RectTransform Component
     * @note The Rect Transform component is the 2D layout counterpart of the Transform component.
     *       Where Transform represents a single point, Rect Transform represent a rectangle that a UI element can be placed inside.
     *       If the parent of a Rect Transform is also a Rect Transform, the child Rect Transform can also specify how it should be positioned and sized relative to the parent rectangle.
    */
    class RectTransform : public Transform {
    public:

        /**
         * @brief Default Constructor
        */
        RectTransform();

        /**
         * @brief Default Destructor
        */
        ~RectTransform() override;

        // Pos
        void SetPos(Vector3 pos) { m_pos = pos; }
        Vector3 GetPos() { return m_pos; }

        // Size
        void SetSize(Vector2 size) { m_size = size; }
        Vector2 GetSize() { return m_size; }

        /**
         * @brief Call before object resource change
        */
        void PostResourceModify() override;

        /**
         * @brief Call before object resource loaded
         * when instantiate prefab, add component, resource loaded etc
         * after call resource load completed
        */
        void PostResourceLoaded() override;

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

        /**
         * @brief Called every frame, only editor mode
        */
        void OnEditorUpdate() override;

    private:

        /**
         * @brief Update RectTransform
        */
        void UpdateRectTransform();

        /**
         * @brief Update Canvas
        */
        void UpdateCanvas();

        /**
         * @brief Owner Canvas pointer
        */
        UICanvas* m_canvas = nullptr;

        /**
         * @brief ui resolution pos, if (0,0) is top-left 
        */
        Vector3 m_pos;

        /**
         * @brief  ui resolution size if(10,10) is 10 pixel width and height
        */
        Vector2 m_size;

        /**
         * @brief rectTransform matrix
        */
        Matrix m_rectTransformMatrix;

        RTTR_ENABLE(Transform)
    };
}
